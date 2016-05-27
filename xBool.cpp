//////////////////////////////////////////////////////////////////////////
// ���������� ������� xBool

// jini
#include "jini.h"

// ������������ ������������ ��� xBool ��������� ��� �������� �����.
using namespace xBool;

//////////////////////////////////////////////////////////////////////////
// Pair

// ����������� �� ���������.
Pair::Pair ( const wchar_t* s_true, const wchar_t* s_false )
    : _true ( s_true ), _false ( s_false )
{
}; // Pair

// ���������� ������� ����������� ��������.
const wchar_t* Pair::get_synonym ( bool value ) const
{
    return this ? ( value ? _true.c_str ( ) : _false.c_str ( ) ) : ( value ? L"1" : L"0" );
}; // get_synonym

// ���������� �������� ��������, ���� �� ��������� � ������� ����.
bool Pair::get_value ( const wchar_t* synonym, bool& value ) const
{
    if ( this )
    {
        if ( xString::iequal ( _true, synonym ) )
            value = true;
        else if ( xString::iequal ( _false, synonym ) )
            value = false;
        else
            goto l_retf;
        goto l_rett;
    };
l_retf:
    return false;
l_rett:
    return true;
}; // get_value

//////////////////////////////////////////////////////////////////////////
// PairEx

// ����������� �� ���������.
Synonyms::PairEx::PairEx ( unsigned short id, const wchar_t* s_true, const wchar_t* s_false )
    : Pair ( s_true, s_false )
    , _id ( id )
{
}; // PairEx

//////////////////////////////////////////////////////////////////////////
// Synonyms

// ��������������� ���� ��� ��������.
typedef std::vector < Synonyms::PairEx >::const_iterator vpi;

// ����������� �� ���������.
Synonyms::Synonyms ( bool def /* = true */ )
    : _synonyms ( )
{
    if ( def )
    {
        _synonyms.push_back ( PairEx ( b_1_0, L"1", L"0" ) );
        _synonyms.push_back ( PairEx ( b_t_f, L"true", L"false" ) );
        _synonyms.push_back ( PairEx ( b_y_n, L"yes", L"no" ) );
        _synonyms.push_back ( PairEx ( b_o_o, L"on", L"off" ) );
        _synonyms.push_back ( PairEx ( b_e_d, L"enabled", L"disabled" ) );
    };
}; // Synonyms

// ����������.
Synonyms::~Synonyms ( void )
{
    erase ( );
}; // ~Synonyms

// ��������� ���� ��������� � ������.
Pair* Synonyms::add_pair ( unsigned short id, const wchar_t* s_true, const wchar_t* s_false )
{
    Pair* pPair = nullptr;
    vpi cur = _synonyms.cbegin ( ), end = _synonyms.cend ( );
    bool b;
    for ( vpi cur = _synonyms.cbegin ( ),
        end = _synonyms.cend ( ); cur != end; cur++ )
        if ( cur->_id == id || cur->get_value ( s_true, b ) || cur->get_value ( s_false, b ) )
            break;
    if ( cur == end )
    {
        _synonyms.push_back ( PairEx ( id, s_true, s_false ) );
        pPair = (Pair*) &_synonyms.back ( );
    };
    return pPair;
}; // add_pair

// ���������� ��������� �� ���� ���������.
Pair* Synonyms::get_pair ( unsigned short id ) const
{
    Pair* pPair = nullptr;
    for ( vpi cur = _synonyms.cbegin ( ), end = _synonyms.cend ( ); cur != end; cur++ )
        if ( cur->_id == id )
        {
            pPair = (Pair*) &*cur;
            break;
        };
    return pPair;
}; // get_pair

// ���������� ��������� �� ���� ���������.
Pair* Synonyms::get_pair ( const wchar_t* synonym ) const
{
    Pair* pPair = nullptr;
    bool b;
    for ( vpi cur = _synonyms.cbegin ( ), end = _synonyms.cend ( ); cur != end; cur++ )
        if ( cur->get_value ( synonym, b ) )
        {
            pPair = (Pair*) &*cur;
            break;
        };
    return pPair;
}; // get_pair

// ������� ���� �� ������.
void Synonyms::erase ( unsigned short id )
{
    for ( vpi cur = _synonyms.cbegin ( ), end = _synonyms.cend ( ); cur != end; cur++ )
        if ( cur->_id == id )
        {
            _synonyms.erase ( cur );
            break;
        };
}; // erase

// ������� ��� ���� �� ������.
void Synonyms::erase ( void )
{
    _synonyms.erase ( _synonyms.cbegin ( ), _synonyms.cend ( ) );
}; // erase
