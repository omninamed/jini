//////////////////////////////////////////////////////////////////////////
// jini - json-�������� ������ ������ �������������.
//
// �����������: ����
// ����:        23.05.2016
// ���������:   UNICODE
// ������:      0.3 build 27
//
// ����������� ������:
// - ������ ���������������� �� "����" ������ ��������������� ��� ���������� ������� json
//   ���������� ��� �� json ������ ��� ��������.
// - ������� �� ATL, ������ �� �� ������ STL
// - ��������� ��������� �������� ������.
// - ��������� ������ ��������� ��� ���������� ��������.
// - ����������� ������ � ��������� ������ �� ����.
// - ����� ������ ������ ����������� ����� �� ���������� �� ������ �����.
// - ���������� ���� ����������� ��� ������ ������������� �� ������ ������, ������ ����� �� ��� ����... ;)
// - ����������� ��������� ������ ���������� � �����������. ��� ������ ������ ������������.
//
// ����������� ������:
// - �������� ��������� - ������� ��������� ������������� ������� � �����.
// - ������� � ���������� �������������� �������� ���.
// - ������������, ����� ������� ��������.
// - ������ �Ѩ ��� ����� ��������� ��������� ��� ������ ����������.
// - ������������ ������� � ���� �����������/����������� ������� �� ����� (���������������� ������)
// - ��� ���������� �������� � ������������� ������, �� �� ����������� ����� ����������
//   ������ ����� �������� (������� ��������, ���� ��� ������)

#pragma once

// std::locale
#include <locale>
// std::wstring
#include <string>
// std::vector
#include <vector>
// ����������� ���������.
#include <algorithm>

// �������� �� ��������.
namespace xString
{
    // ��������������� ������� �����.
    typedef std::vector < std::wstring > wsvector;

    // ���������� ��� ������ ��� ����� �������� ��������� ������� ������.
    inline bool iequal ( const std::wstring& s1, const std::wstring& s2 )
    {
        return ( s1.size ( ) == s2.size ( ) ) && std::equal ( s1.begin ( ), s1.end ( ), s2.begin ( ),
            [] ( const wchar_t& ch1, const wchar_t& ch2 ) { return ::_towlower_l ( ch1, nullptr ) == ::_towlower_l ( ch2, nullptr ); } );
    }; // iequal

    // ������ ���� ��������.
    inline std::wstring repl ( std::wstring str, const std::wstring substr, const std::wstring replstr )
    {
        for ( std::size_t i = 0; ( i = str.find ( substr, i ) ) != std::wstring::npos; i += replstr.size ( ) )
            str.replace ( i, substr.size ( ), replstr );
        return str;
    }; // Replace

    // ���������� ������.
    inline void split ( const std::wstring& s, const std::wstring& delim, wsvector& v )
    {
        if ( delim.empty ( ) )
            v.push_back ( s );
        else
        {
            std::wstring::const_iterator sub = s.cbegin ( );
            for (;;)
            {
                std::wstring::const_iterator end =
                    std::search ( sub, s.cend ( ), delim.cbegin ( ), delim.cend ( ) );
                v.push_back ( std::wstring ( sub, end ) );
                if ( end == s.end ( ) )
                    break;
                sub = end + delim.size ( );
            };
        };
    }; // split
}; // xString

// ���������� ��������.
namespace xBool
{
    // ���� ��������� ��� bool
    class Pair
    {
    public:

        // ������� ��� true
        std::wstring _true;
        // ������� ��� false
        std::wstring _false;

        // ����������� �� ���������.
        Pair ( const wchar_t* s_true, const wchar_t* s_false );

        // ���������� ������� ����������� ��������.
        const wchar_t* get_synonym ( bool value ) const;
        // ���������� �������� ��������, ���� �� ��������� � ������� ����.
        bool get_value ( const wchar_t* synonym, bool& value ) const;
    }; // Pair

    // ���������� ��� ��������� ���������� ��������.
    enum
    {
        // 1 � 0
        b_1_0,
        // true � false
        b_t_f,
        // yes � no
        b_y_n,
        // on � off
        b_o_o,
        // enabled � disabled
        b_e_d
    };

    // ������ ��������� ��� bool
    class Synonyms
    {
    public:

        // ���� ��������� � ���������������.
        struct PairEx : public Pair
        {
            // ������������� ����.
            unsigned short _id;

            // ����������� �� ���������.
            PairEx ( unsigned short id, const wchar_t* s_true, const wchar_t* s_false );
        }; // PairEx

        // ������ ��������� ��� bool
        std::vector < PairEx > _synonyms;
    
        // ����������� �� ���������.
        Synonyms ( bool def = true );
        // ����������.
        ~Synonyms ( void );

        // ��������� ���� ��������� � ������.
        Pair* add_pair ( unsigned short id, const wchar_t* s_true, const wchar_t* s_false );

        // ���������� ��������� �� ���� ���������.
        Pair* get_pair ( unsigned short id ) const;
        // ���������� ��������� �� ���� ���������.
        Pair* get_pair ( const wchar_t* synonym ) const;

        // ������� ���� �� ������.
        void erase ( unsigned short id );
        // ������� ��� ���� �� ������.
        void erase ( void );
    }; // Synonyms
}; // xBool

// jini
namespace jini
{
    // ��������� �������� ������.
    struct tagSettings
    {
        // ������� �������� � ������ ������������� ���������.
        bool bSkipWhitespaces;  // �� ��������� false
        // ������ ������� ��� ��������� ��������.
        unsigned short nIndent; // �� ��������� 2

        // ������ ��� ��������� ��� jini
        xBool::Synonyms xBooleans; // �� ��������� ������� ������.

        // ����������� �� ���������.
        tagSettings ( void );
    }; // tagSettings

    // ��������� ������.
    extern tagSettings Settings;

    //////////////////////////////////////////////////////////////////////////

    // ������� ����� ��� ����� �������� json | jini
    class Base
    {
    public:

        // ��������� �� ������������ ������.
        Base* _parent;

        enum
        {
            // ���� ["���":"��������"]
            jPair = 1,
            // ������ ["���"{...}]
            jObject
        };

        // ���.
        unsigned short _type;

        // ���.
        std::wstring _name;
    
        // ����������� �� ���������.
        Base ( unsigned short type, const wchar_t* name, Base* parent = nullptr );
        // ����������� �����������.
        Base ( const Base& source, Base* parent = nullptr );
        // ����������.
        virtual ~Base ( void );

        // ������ ������ ����� �������� �������.
        virtual Base* clone ( Base* parent = nullptr ) const = 0;

        // ���������� �������� �������.
        Base* get_root ( void ) const;
    }; // Base

    // ������������ ���� ["���":"��������"]
    class Pair : public Base
    {
    public:

        // RAW-��������.
        std::wstring _value;
    
        // ����������� �� ���������.
        Pair ( const wchar_t* name = L"", const wchar_t* value = L"", Base* parent = nullptr );
        // ����������� �����������.
        Pair ( const Pair& source, Base* parent = nullptr );
        // ����������.
        ~Pair ( void );

        // ������ ������ ����� �������� �������.
        virtual Base* clone ( Base* parent = nullptr ) const;

        // �������������� ��������.
        void format ( const wchar_t* fmt, ... );

        // ������������� ��������� ��������.
        void set_string ( const wchar_t* value );
        // ������������� ���������� ��������.
        void set_bool ( bool value, unsigned short pair_id = xBool::b_1_0 );
        // ������������� ������������� ��������.
        void set_int ( int value, int radix = 10 );
        // ������������� �������� � ��������� ������.
        void set_float ( float value, int precision = 3 );

        // ���������� ��������� ��������.
        const wchar_t* get_string ( const wchar_t* def = L"" ) const;
        // ���������� ���������� ��������.
        bool get_bool ( bool def = false ) const;
        // ���������� ������������� ��������.
        int get_int ( int def = 0, int radix = 0 ) const;
        // ���������� �������� � ��������� ������.
        float get_float ( float def = 0.0f ) const;
    }; // Pair

    // ������������ ������ ["���"{...}]
    class Object : public Base
    {
    public:

        // ������ �������� ���������.
        std::vector < Base* > _childs;
    
        // ����������� �� ���������.
        Object ( const wchar_t* name = L"", Base* parent = nullptr );
        // ����������� �� ���������.
        Object ( const Object& source, Base* parent = nullptr );
        // ����������.
        ~Object ( void );

        // ������ ������ ����� �������� �������.
        virtual Base* clone ( Base* parent = nullptr ) const;

        // ��������� ����.
        Pair* add_pair ( const wchar_t* name, const wchar_t* value );
        // ��������� ������.
        Object* add_object ( const wchar_t* name );

        // ���������� ������� ��� �������� ����.
        Base* get_element ( const wchar_t* n_o_p, const wchar_t* delim = nullptr ) const;
        // ���������� ����.
        Pair* get_pair ( const wchar_t* n_o_p, const wchar_t* delim = nullptr ) const;
        // ���������� ������.
        Object* get_object ( const wchar_t* n_o_p, const wchar_t* delim = nullptr ) const;

        // ���������, ���� �� ������� � ������� �������.
        bool is_exist ( const wchar_t* name ) const;

        // ������� �������� �������.
        void erase ( const wchar_t* name );
        // ������� ��� �������� ��������.
        void erase ( void );
    }; // Object

    // ��������� ���� .jini
    bool read ( Object& lOut, const std::locale& loc );
    // ���������� ���� .jini
    bool write ( const Object& lIn, const std::locale& loc );
}; // jini
