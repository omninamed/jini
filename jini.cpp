//////////////////////////////////////////////////////////////////////////
// ���������� ������� ������� ������ � ��� ��������.

// JINI
#include "jini.h"

// ������������ ������������ ��� jini ��������� ��� �������� �����.
using namespace jini;

// ����������� �� ���������.
tagSettings::tagSettings ( void )
    : bSkipWhitespaces ( false )
    , nIndent ( 2 )
    , xBooleans ( true )
{
}; // tagSettings

// ������������� �������� �� ���������.
tagSettings jini::Settings;

