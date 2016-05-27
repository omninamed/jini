//////////////////////////////////////////////////////////////////////////
// Реализация класса jini::Base

// jini
#include "jini.h"

// Использовать пространство имён jini глобально для текущего файла.
using namespace jini;

// Конструктор по умолчанию.
Base::Base ( unsigned short type, const wchar_t* name, Base* parent /* = nullptr */ )
    : _type ( type )
    , _name ( name )
    , _parent ( parent )
{
}; // Base

// Конструктор копирования.
Base::Base ( const Base& source, Base* parent /* = nullptr */ )
    : _type ( source._type )
    , _name ( source._name )
    , _parent ( parent )
{
}; // Base

// Деструктор.
/* virtual */ Base::~Base ( void )
{
}; // ~Base

// Возвращает корневой элемент.
Base* Base::get_root ( void ) const
{
    Base* _temp = _parent;
    if ( _temp )
        while ( _temp->_parent )
            _temp = _temp->_parent;
    return _temp;
}; // get_root
