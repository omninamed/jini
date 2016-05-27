//////////////////////////////////////////////////////////////////////////
// ���������� ������ jini::Object

// jini
#include "jini.h"

// ������������ ������������ ��� jini ��������� ��� �������� �����.
using namespace jini;

// ��������������� ��������� ��� ��������.
typedef std::vector < Base* >::const_iterator vci;

// ����� ��������.
vci vfind ( const std::vector < Base* >& v, const wchar_t* name )
{
    vci cur = v.cbegin ( );
    if ( name )
        for ( vci end = v.cend ( ); cur != end; cur++ )
            if ( xString::iequal ( (*cur)->_name, name ) == 0 )
                break;
    return cur;
}; // vfind

// ����������� �� ���������.
Object::Object ( const wchar_t* name /*  = L"" */, Base* parent /* = nullptr */ )
    : Base ( Base::jObject, name, parent )
{
}; // Object

// ����������� �� ���������.
Object::Object ( const Object& source, Base* parent /* = nullptr */ )
    : Base ( source, parent )
{
    for ( vci cur = source._childs.cbegin ( ), end = source._childs.cend ( ); cur != end; cur++ )
        _childs.push_back ( (*cur)->clone ( ) );
}; // Object

// ����������.
Object::~Object ( void )
{
    erase ( );
}; // ~Object

// ������ ������ ����� �������� �������.
/* virtual */ Base* Object::clone ( Base* parent /* = nullptr */ ) const
{
    return new Object ( *this, parent );
}; // clone

// ��������� ����.
Pair* Object::add_pair ( const wchar_t* name, const wchar_t* value )
{
    Pair* _pair = nullptr;
    if ( this && vfind ( _childs, name ) == _childs.cend ( ) )
        _childs.push_back ( _pair = new Pair ( name, value, this ) );
    return _pair;
}; // add_pair

// ��������� ������.
Object* Object::add_object ( const wchar_t* name )
{
    Object* _object = nullptr;
    if ( this && vfind ( _childs, name ) == _childs.cend ( ) )
        _childs.push_back ( _object = new Object ( name, this ) );
    return _object;
}; // add_object

// ���������� ������� ��� �������� ����.
Base* Object::get_element ( const wchar_t* n_o_p, const wchar_t* delim /* = nullptr */ ) const
{
    // ��������� �� �������� ������� - ��������� ������.
    Base* _elem = nullptr;
    // �������� �������� ������� � ������� �����/����.
    if ( this && n_o_p )
    {
        // ������ ��������� ����.
        xString::wsvector _path_elements;
        // ��������� ���� �� ������������.
        xString::split ( n_o_p, delim ? delim : L"", _path_elements );
        // ��������� �� ������� �������������� �������.
        Base* _cur_elem = (Base*) this;
        // ���� �������� ��������� ����.
        for ( xString::wsvector::const_iterator _pe_cur = _path_elements.cbegin ( ), _pe_end = _path_elements.cend ( ); _pe_cur != _pe_end; _pe_cur++ )
        {
            // ����� ��� �� ��������, �� ��������� ��������� ������� �������� �����, � ���� ��� ����� ��������� �������� ���������.
            if ( _cur_elem->_type == Base::jPair )
            {
                // �������� ���������.
                _cur_elem = nullptr;
                // ��������� �����.
                break;
            };
            // �������� ��� ��� ��������.
            Object* pObject = (Object*) _cur_elem;
            // ���� ������ �����������.
            for ( vci _c_cur = pObject->_childs.cbegin ( ), _c_end = pObject->_childs.cend ( ); _c_cur != _c_end; _c_cur++ )
                // �������� ���������� ���.
                if ( xString::iequal ( (*_c_cur)->_name, *_pe_cur ) )
                    // �������� ���������.
                    _cur_elem = *_c_cur;
        };
        // �������� ���������, ������ ���� �� �� ��������� �� ������� ������.
        _elem = _cur_elem != this ? _cur_elem : nullptr;
    };
    // ���������� ���������.
    return _elem;
}; // get_element

// ���������� ����.
Pair* Object::get_pair ( const wchar_t* n_o_p, const wchar_t* delim /* = nullptr */ ) const
{
    Base* _elem = get_element ( n_o_p, delim );
    return _elem && _elem->_type == Base::jPair ? (Pair*) _elem : nullptr;
}; // get_pair

// ���������� ������.
Object* Object::get_object ( const wchar_t* n_o_p, const wchar_t* delim /* = nullptr */ ) const
{
    Base* _elem = get_element ( n_o_p, delim );
    return _elem && _elem->_type == Base::jObject ? (Object*) _elem : nullptr;
}; // get_object

// ���������, ���� �� ������� � ������� �������.
bool Object::is_exist ( const wchar_t* name ) const
{
    return vfind ( _childs, name ) != _childs.cend ( );
}; // is_exist

// ������� �������� �������.
void Object::erase ( const wchar_t* name )
{
    if ( this )
        for ( vci cur = _childs.cbegin ( ), end = _childs.cend ( ); cur != end; cur++ )
        {
            Base* _elem = *cur;
            if ( xString::iequal ( _elem->_name, name ) )
            {
                delete _elem;
                _childs.erase ( cur );
                break;
            };
        };
}; // erase

// ������� ��� �������� ��������.
void Object::erase ( void )
{
    if ( this )
    {
        for ( vci cur = _childs.cbegin ( ), end = _childs.cend ( ); cur != end; cur++ )
            delete *cur;
        _childs.erase ( _childs.cbegin ( ), _childs.cend ( ) );
    };
}; // erase
