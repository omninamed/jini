//////////////////////////////////////////////////////////////////////////
// ���������� ������ jini::Read

// jini
#include "jini.h"

// std::wifstream � std::wofstream
#include <fstream>

// ������������ ������������ ��� jini ��������� ��� �������� �����.
using namespace jini;

// ��������� ���� .jini
bool jini::read ( Object& lOut, const std::locale& loc )
{
    // ��������� ������.
    bool bResult = false;
    // ��������� ���� � ������������.
    std::wifstream ifile ( lOut._name, std::ios::in );
    // ������?
    if ( ifile.is_open ( ) )
    {
        // ������ ��� �����.
        ifile.imbue ( loc );

        std::wstring
            // ����� ������.
            buf,
            // ��������� ����� ��� ������������� ���������.
            multiline,
            // ����� ��� ����� ���������� ��������.
            name,
            // ����� ��� �������� ����.
            value;

        int
            // ������� ������.
            nLine = 1,
            // ������� �������.
            nColumn,
            // ����� ���������� �������� (�������� � ������� ������)
            nColumnCount,
            // ������� ����������� ������������ ��������.
            nSkipDepth = 0;

        bool
            // ������� �������������� ��������.
            bIsMultiline = false,
            // ������� ���������� ����� ��������.
            bIsNamed = false,
            // ������� ���������� ����������� ����.
            bIsPair = false;

        // ������� �������������� ������.
        Object* pCurrent = &lOut;

        // ������ �� ���������!
        while ( ifile.good ( ) )
        {
            // ���������� ������ �� �����.
            std::getline ( ifile, buf );
            // �������� �����.
            const wchar_t* pBuffer = buf.c_str ( );
            // ��������� �������.
            nColumn = 0;
            // ����� ��������.
            nColumnCount = buf.size ( );
            // ������������ ������ ��������� ������.
            while ( nColumn < nColumnCount )
            {
                // ������ ��� ��������.
                const wchar_t& ch = pBuffer [ nColumn ];
                // ����� ��� ������ �� ����������� �������?
                if ( bIsMultiline && !Settings.bSkipWhitespaces )
                    // ��������� � ����������� ���������, ������� ���� �
                    // ���������� �� ������ ������� �������� � ������ �����.
                    goto l_process_literal;

                // ������� �������� ����� ������:

                if ( std::isspace < wchar_t > ( ch, loc ) )
                {
                    nColumn++;
                    continue;
                };

                // ��� ��� � ���?
                switch ( ch )
                {
                case L'\"': // ������ ��������.
                    {
l_process_literal:
                        // ��������� ������ ��� ������ �������� ��������.
                        int nFindex = bIsMultiline ? nColumn : ++nColumn;
                        // ����� ������������ �������.
                        while ( nFindex < nColumnCount )
                        {
                            // �������? ��� ������ ��� �� ������������?
                            if ( pBuffer [ nFindex ] == L'\"' && ( nFindex - 1 < 0 || pBuffer [ nFindex - 1 ] != L'\\' ) )
                                // ����� �������� ������, ��������� ���� ������.
                                break;
                            nFindex++; // ���� ������ ������.
                        };

                        // ��������� ���� �������������� ��������.
                        bool old_bIsMultiline = bIsMultiline;
                        // ��������� ����.
                        bIsMultiline = nFindex == nColumnCount;
                        // ������� �� ��������?
                        if ( bIsMultiline )
                            // ������������ � ���������������� ������.
                            multiline.append ( pBuffer + nColumn );
                        else
                        {
                            // ������ �� ����� ��� �������� � ����������� �� ��������� �����������.
                            std::wstring& literal_link = bIsPair ? value : name;
                            // ����� �� �����? wtf?
                            if ( !literal_link.empty ( ) )
                            {
                                // ������ ���������� - ��������� ��������� ������.

                                // ����� ��� ������ ����������� ����.
                                if ( bIsPair )
                                {
                                    // ������ ��� ���������� �������� ����.
                                };
                                // ������� �����.
                                multiline.erase ( );
                                // ��������� � ����� �������� ������.
                                goto l_skip_literla_error;
                            }
                            else if ( old_bIsMultiline ) // �������� ��� ��� ����� �������������� ��������.
                            {
                                // ������������ �����.
                                literal_link.append ( multiline );
                                // ������� ����������� �����.
                                multiline.erase ( );
                            };
                            // ��������� ��������� ������.
                            literal_link.append ( pBuffer + nColumn, nFindex - nColumn );
                            // ����� ��� ������ ����������� ����?
                            if ( bIsPair )
                            {
                                // ��� �����!
                                if ( !bIsNamed )
                                {
                                    // ������ ���������� - ���������� ����.
                                    // �.�. ��� �� ������, � ������ �����������.
                                    // Unexpected pair value ��� �������.
                                    // ������� ��� ��������, ����� ���������� ����.
                                };
                                // ��������� ���� ������ ���� ��� �� ����������� � ������������� �������.
                                if ( !nSkipDepth )
                                {
                                    // �������� �������� �������.
                                    Base* pElement = pCurrent->get_element ( name.c_str ( ) );
                                    // ����?
                                    if ( pElement )
                                    {
                                        // ��������.

                                        // ��������� ��� ��������.
                                        switch ( pElement->_type )
                                        {
                                        case Base::jPair: // ����.
                                            // ��������� ��������.
                                            ((Pair*)pElement)->_value = value.c_str ( );
                                            break;
                                        case Base::jObject: // ������.
                                        default: // ���?
                                            {
                                                // ������ - ��� ���������� � ��������� ��������� ���������,
                                                // �� �������� ������� �� �������� �����.
                                            };
                                        };
                                    }
                                    else
                                        // ��������� ������ ���� � ������� �������������� ������.
                                        pCurrent->_childs.push_back ( new Pair ( name.c_str ( ), value.c_str ( ), pCurrent ) );
                                };
                                // ������� ������.
                                name.erase ( ); value.erase ( );
                                // ���������� �����.
                                bIsNamed = false; bIsPair = false;
                            }
                            else if ( bIsNamed )
                            {
                                // ���?
                                // ���� �� ���� ������ ������� �� ������
                                // �������� ��-�� �������� l_skip_literla_error
                            }
                            else
                                // ������ ���������.
                                bIsNamed = true;
                        };
l_skip_literla_error:
                        // ��������� �������� ������� ��� ����������� �������.
                        nColumn = nFindex + 1;
                        // ��������� ��������, �.�. ��� ������ ��� ����������.
                        continue;
                    };
                    break;
                case L':': // ����������� ����.
                    {
                        if ( nSkipDepth )
                        {
                            // �������.
                        };
                        if ( bIsPair )
                        {
                            // ������ - ������ ����������� ':'
                        };
                        if ( !bIsNamed )
                        {
                            // ������ - � ���� ��� �����.
                        };
                        bIsPair = true;
                    };
                    break;
                case L',': // ���������� ��������� ����������� ������.
                    break;
                case L'{': // ������ �������.
                    {
                        // ������� ��������� � �������������� �����.
                        if ( nSkipDepth )
                        {
                            // ���� ������� �����������.
                            nSkipDepth++;
                            // ��������� � ����� �����������.
                            goto l_skip_this_object;
                        };

                        if ( bIsPair )
                        {
                            // ������ ���������� - '{' ����� ':'

                            // ���������� ���� ���� � ����� ��������� ��� ��� ������������� ������ ;)
                            bIsPair = false;
                        };
                        if ( !bIsNamed )
                        {
                            // ����������� ���.

                            // ��� �� ����������� ������, ���������� ������� ���������.
                        };

                        // ���������, ���� �� ����� ������ � ������.
                        Base* pElement = pCurrent->get_element ( name.c_str ( ) );
                        // ����?
                        if ( pElement )
                        {
                            // ��������.

                            // ��������� ���.
                            switch ( pElement->_type )
                            {
                            case Base::jObject: // ������.
                                // �������� ��������� ��� ����������/���������� ������.
                                pCurrent = (Object*) pElement;
                                break;
                            default:
                                {
                                    // ������ - ��� ���������� � ��������� ��������� ���������,
                                    // �� �������� ������� �� �������� ��������.

                                    // ������� ������� ����������� ��� ����������� �������� ����� �������.
                                    nSkipDepth++;
                                    // ���� �������.
                                    nColumn++;
                                    // ��������� �������� �������.
                                    continue;
                                };
                            };
                        }
                        else
                        {
                            // ����� ������.

                            // �������� ��������� �� ������� ������, �� ����� ������������.
                            Object* pParent = pCurrent;
                            // ������ ����� ������.
                            pCurrent = new Object ( name.c_str ( ), pParent );
                            // ��������� ��� � ������.
                            pParent->_childs.push_back ( pCurrent );
                        };
l_skip_this_object:
                        // ������� ���.
                        name.erase ( );
                        // ���������� ���� ������� �����.
                        bIsNamed = false;
                    };
                    break;
                case L'}': // ����� �������.
                    {
                        // ���� ����� ��� ����������� ������� �������.
                        if ( nSkipDepth )
                            // ��������� ������� �����������.
                            nSkipDepth--;
                        else if ( pCurrent->_parent )
                            pCurrent = (Object*) pCurrent->_parent;
                        else
                        {
                            // ������, ��� ������� ��� �������.
                        };
                    };
                    break;
                default: // ������������ �������������� �������.
                    {
                        // ���������� ������. ���� �� ������ �� ;)
                    };
                };
                // ������� ������� ��������.
                nColumn++;
            };
            // ���� ������ ����� �� ��� �� ���������.
            if ( ifile.good ( ) )
                // ������� ������� �����.
                nLine++;
        };
        // ��������� �������� �����.
        ifile.close ( );
        // ��, ���� ������� ���� ���-��.
        bResult = nLine > 1;
    };
    // ���������� ���������.
    return bResult;
}; // read
