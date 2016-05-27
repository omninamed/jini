//////////////////////////////////////////////////////////////////////////
// ���������� ������ jini::Base

// jini
#include "jini.h"

// ������������ ������������ ��� jini ��������� ��� �������� �����.
using namespace jini;

// ����������� �� ���������.
Base::Base ( unsigned short type, const wchar_t* name, Base* parent /* = nullptr */ )
    : _type ( type )
    , _name ( name )
    , _parent ( parent )
{
}; // Base

// ����������� �����������.
Base::Base ( const Base& source, Base* parent /* = nullptr */ )
    : _type ( source._type )
    , _name ( source._name )
    , _parent ( parent )
{
}; // Base

// ����������.
/* virtual */ Base::~Base ( void )
{
}; // ~Base

// ���������� �������� �������.
Base* Base::get_root ( void ) const
{
    Base* _temp = _parent;
    if ( _temp )
        while ( _temp->_parent )
            _temp = _temp->_parent;
    return _temp;
}; // get_root
