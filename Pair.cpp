//////////////////////////////////////////////////////////////////////////
// Реализация класса jini::Pair

// jini
#include "jini.h"

// Доступ к аргументам.
#include <cstdarg>
// std::wstringstream
#include <sstream>
// Манипуляторы стандартных потоков.
#include <iomanip>

// Использовать пространство имён jini глобально для текущего файла.
using namespace jini;

// Конструктор по умолчанию.
Pair::Pair ( const wchar_t* name /* = L"" */, const wchar_t* value /* = L"" */, Base* parent /* = nullptr */ )
    : Base ( Base::jPair, name, parent )
    , _value ( value )
{
}; // Pair

// Конструктор копирования.
Pair::Pair ( const Pair& source, Base* parent /* = nullptr */ )
    : Base ( source, parent )
    , _value ( source._value )
{
}; // Pair

// Деструктор.
Pair::~Pair ( void )
{
}; // ~Pair

// Создаёт точную копию текущего объекта.
/* virtual */ Base* Pair::clone ( Base* parent /* = nullptr */ ) const
{
    return new Pair ( *this, parent );
}; // clone

// Форматирование значения.
void Pair::format ( const wchar_t* fmt, ... )
{
    va_list args;
    va_start ( args, fmt );
    int nLength = _vscwprintf ( fmt, args );
    if ( nLength > 0 )
    {
        _value.resize ( nLength + 1 );
        vswprintf ( &_value [ 0 ], nLength + 1, fmt, args );
    }
    else
        _value.erase ( );
    va_end ( args );
}; // FormatValue

// Устанавливает строковое значение.
void Pair::set_string ( const wchar_t* value )
{
    if ( this )
        _value = value;
}; // set_string

// Устанавливает логическое значение.
void Pair::set_bool ( bool value, unsigned short pair_id /* = xBool::b_1_0 */ )
{
    if ( this )
        _value = Settings.xBooleans.get_pair ( pair_id )->get_synonym ( value );
}; // set_bool

// Устанавливает целочисленное значение.
void Pair::set_int ( int value, int radix /* = 10 */ )
{
    if ( this )
    {
        std::wstringstream converter;
        converter << std::setbase ( radix );
        std::uppercase ( converter );
        converter << value;
        converter >> _value;
        if ( radix == 16 )
            _value.insert ( 0, L"0x" );
    };
}; // set_int

// Устанавливает значение с плавающей точкой.
void Pair::set_float ( float value, int precision /* = 3 */ )
{
    if ( this )
    {
        std::wstringstream converter;
        converter.precision ( precision );
        converter.fill ( '0' );
        converter.setf ( std::ios::fixed, std::ios::floatfield );
        converter << value;
        converter >> _value;
    };
}; // set_float

// Возвращает строковое значение.
const wchar_t* Pair::get_string ( const wchar_t* def /* = L"" */ ) const
{
    if ( this )
        def = _value.c_str ( );
    return def;
}; // get_string

// Возвращает логическое значение.
bool Pair::get_bool ( bool def /* = false */ ) const
{
    if ( this )
        Settings.xBooleans.get_pair ( _value.c_str ( ) )->get_value ( _value.c_str ( ), def );
    return def;
}; // get_bool

// Возвращает целочисленное значение.
int Pair::get_int ( int def /* = 0 */, int radix /* = 0 */ ) const
{
    if ( this )
    {
        const wchar_t* pValue = _value.c_str ( );
        wchar_t* pEnd = nullptr;
        int nValue = (int) ::wcstol ( pValue, &pEnd, radix );
        if ( pEnd > pValue )
            def = nValue;
    };
    return def;
}; // get_int

// Возвращает значение с плавающей точкой.
float Pair::get_float ( float def /* = 0.0f */ ) const
{
    if ( this )
    {
        const wchar_t* pValue = _value.c_str ( );
        wchar_t* pEnd = nullptr;
        float fValue = (float) ::wcstod ( pValue, &pEnd );
        if ( pEnd > pValue )
            def = fValue;
    };
    return def;
}; // get_float
