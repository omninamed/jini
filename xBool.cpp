//////////////////////////////////////////////////////////////////////////
// Реализация классов xBool

// jini
#include "jini.h"

// Использовать пространство имён xBool глобально для текущего файла.
using namespace xBool;

//////////////////////////////////////////////////////////////////////////
// Pair

// Конструктор по умолчанию.
Pair::Pair ( const wchar_t* s_true, const wchar_t* s_false )
    : _true ( s_true ), _false ( s_false )
{
}; // Pair

// Возвращает синоним логического значения.
const wchar_t* Pair::get_synonym ( bool value ) const
{
    return this ? ( value ? _true.c_str ( ) : _false.c_str ( ) ) : ( value ? L"1" : L"0" );
}; // get_synonym

// Возвращает значение синонима, если он относится к текущей паре.
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

// Конструктор по умолчанию.
Synonyms::PairEx::PairEx ( unsigned short id, const wchar_t* s_true, const wchar_t* s_false )
    : Pair ( s_true, s_false )
    , _id ( id )
{
}; // PairEx

//////////////////////////////////////////////////////////////////////////
// Synonyms

// Переопределение типа для удобства.
typedef std::vector < Synonyms::PairEx >::const_iterator vpi;

// Конструктор по умолчанию.
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

// Деструктор.
Synonyms::~Synonyms ( void )
{
    erase ( );
}; // ~Synonyms

// Добавляет пару синонимов в список.
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

// Возвращает указатель на пару синонимов.
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

// Возвращает указатель на пару синонимов.
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

// Удаляет пару из списка.
void Synonyms::erase ( unsigned short id )
{
    for ( vpi cur = _synonyms.cbegin ( ), end = _synonyms.cend ( ); cur != end; cur++ )
        if ( cur->_id == id )
        {
            _synonyms.erase ( cur );
            break;
        };
}; // erase

// Удаляет все пары из списка.
void Synonyms::erase ( void )
{
    _synonyms.erase ( _synonyms.cbegin ( ), _synonyms.cend ( ) );
}; // erase
