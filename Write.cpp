//////////////////////////////////////////////////////////////////////////
// ���������� ������ jini::Write

// jini
#include "jini.h"

// std::wifstream � std::wofstream
#include <fstream>
// std::stack
#include <stack>

// ������������ ������������ ��� jini ��������� ��� �������� �����.
using namespace jini;

// ��������������� ��������� ��� ��������.
typedef std::vector < Base* >::const_iterator vci;

// ���������� ���� .jini
bool jini::write ( const Object& lIn, const std::locale& loc )
{
    // ��������� ������.
    bool bResult = false;
    // ��������� ���� � ������������ ��� ������. ������� �����.
    std::wofstream ofile ( lIn._name, std::ios::out | std::ios::trunc );
    // ���� ������?
    if ( ofile.is_open ( ) )
    {
        // ������������� ������.
        ofile.imbue ( loc );

        // ��������� �����.
        struct stack_item
        {
            // ��������� �� ������ jini
            Object* obj;
            // ������� ��������������� ���� �������.
            vci cur;

            // ����������� �� ���������.
            stack_item ( Object* p )
                : obj ( p ), cur ( p->_childs.cbegin ( ) )
            { }; // stack_item
        }; // stack_item

        // ����. ���������� ������, �.�. ���� ��������� �������� �������� � lIn ������� ����� ���������������.
        std::stack < stack_item > _stack;
        // ������� ������� �����.
        stack_item _pos ( (Object*) &lIn );

l_write_object: // ������ ����� ������������ �������.

        { // �������������� ������� ��������� ��� ������������ ������-������.

            // ������ ����� ��� �������� ������� (��������������)
            std::wstring indent ( _stack.size ( ) * Settings.nIndent, L' ' );
            // ������� ;)
            for ( vci end = _pos.obj->_childs.cend ( ); _pos.cur != end; _pos.cur++ )
            {
                // ������ �� ������� ������ ��� ��������.
                Base*const& lpItem = *_pos.cur;
                // ��������� ��� ��������.
                switch ( lpItem->_type )
                {
                case Base::jPair: // ���� "���" : "��������"
                    {
                        // �������������� � Pair ��� ��������.
                        Pair*& lpPair = (Pair*&) lpItem;

                        // ���������� ����.
                        ofile << indent.c_str ( ) << L"\""
                            << xString::repl ( lpPair->_name, L"\"", L"\\\"" ).c_str ( )
                            << L"\" : \""
                            << xString::repl ( lpPair->_value, L"\"", L"\\\"" ).c_str ( )
                            << L"\"";
                        // ���� � ������� ��� ���� ��������.
                        if ( _pos.cur + 1 != end )
                            // ���������� �������-�����������.
                            ofile << L",";
                    };
                    break;
                case  Base::jObject: // ������.
                    {
                        // �������������� � Object ��� ��������.
                        Object*& lpObject = (Object*&) lpItem;

                        // ���������� ��������� �������.
                        ofile << indent.c_str ( )
                            << L"\""
                            << xString::repl ( lpObject->_name, L"\"", L"\\\"" ).c_str ( )
                            << L"\"" << std::endl
                            << indent.c_str ( ) << L"{" << std::endl;

                        // ��� ����������� � ����� ������� ����������
                        // �������� �������� ������ �� ��������� ������.
                        _pos.cur++;
                        // �������� ������� ������ � ����.
                        _stack.push ( _pos );
                        // �������� ������� ������� �����.
                        _pos = stack_item ( lpObject );

                        // ��������� � ��������� ����� �������.
                        goto l_write_object;
                    };
                    break;
                default: // ������������ �������.
                    {
                        // ���?
                    };
                };
                // ���� ��� �� ��������� ������� ��� ���� ��� �� ����.
                if ( _pos.cur + 1 != end || !_stack.empty ( ) )
                    // ���������� ������� ������.
                    ofile << std::endl;
            };

            // ����� �������. ��������� ����.
            if ( !_stack.empty ( ) )
            {
                // ���������� ��������� ������� �����.
                _pos = _stack.top ( );
                // ������� ��� �� �����.
                _stack.pop ( );

                // �������������� ��� �������� ����� ������� �� ���� ��������� ������.
                indent.erase ( 0, Settings.nIndent );
                // ���������� ������� ����� �������.
                ofile << indent.c_str ( ) << L"}";
                // ���� ������� ������ ������� �� �������� ��� �����.
                if ( _pos.cur != _pos.obj->_childs.end ( ) )
                {
                    // ���� ������� �������������� ������ �������� ��������.
                    if ( _pos.obj == &lIn )
                        // ���������� ������� ������.
                        ofile << std::endl;
                    else
                        // ���������� �����������.
                        ofile << L",";
                };
                // ���� ���� ��� �� ������.
                if ( !_stack.empty ( ) )
                    // ���������� ������� ������.
                    ofile << std::endl;

                // ��������� � ��������� ������� ����������� �� �����.
                goto l_write_object;
            };
        };

        // ���� �� ;)
        bResult = true;
        // ��������� �������� ����� ����� ���������� ���� ��������.
        ofile.close ( );
    };
    // ���������� ���������.
    return bResult;
}; // write
