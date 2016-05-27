//////////////////////////////////////////////////////////////////////////
// Реализация базовых методов движка и его настроек.

// JINI
#include "jini.h"

// Использовать пространство имён jini глобально для текущего файла.
using namespace jini;

// Конструктор по умолчанию.
tagSettings::tagSettings ( void )
    : bSkipWhitespaces ( false )
    , nIndent ( 2 )
    , xBooleans ( true )
{
}; // tagSettings

// Инициализация настроек по умолчанию.
tagSettings jini::Settings;

