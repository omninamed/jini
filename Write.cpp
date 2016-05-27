//////////////////////////////////////////////////////////////////////////
// Реализация метода jini::Write

// jini
#include "jini.h"

// std::wifstream и std::wofstream
#include <fstream>
// std::stack
#include <stack>

// Использовать пространство имён jini глобально для текущего файла.
using namespace jini;

// Переопределение итератора для удобства.
typedef std::vector < Base* >::const_iterator vci;

// Записывает файл .jini
bool jini::write ( const Object& lIn, const std::locale& loc )
{
    // Результат метода.
    bool bResult = false;
    // Открываем файл в конструкторе для записи. Очистка файла.
    std::wofstream ofile ( lIn._name, std::ios::out | std::ios::trunc );
    // Файл открыт?
    if ( ofile.is_open ( ) )
    {
        // Устанавливаем локаль.
        ofile.imbue ( loc );

        // Структура стека.
        struct stack_item
        {
            // Указатель на объект jini
            Object* obj;
            // Позиция обрабатываемого поля объекта.
            vci cur;

            // Конструктор по умолчанию.
            stack_item ( Object* p )
                : obj ( p ), cur ( p->_childs.cbegin ( ) )
            { }; // stack_item
        }; // stack_item

        // Стек. Изначально пустой, т.к. файл считается корневым объектом и lIn обязано этому соответствовать.
        std::stack < stack_item > _stack;
        // Текущий элемент стека.
        stack_item _pos ( (Object*) &lIn );

l_write_object: // Начало блока сериализации объекта.

        { // Дополнительная область видимости для пересоздания индент-строки.

            // Отступ полей для текущего объекта (форматирование)
            std::wstring indent ( _stack.size ( ) * Settings.nIndent, L' ' );
            // Поехали ;)
            for ( vci end = _pos.obj->_childs.cend ( ); _pos.cur != end; _pos.cur++ )
            {
                // Ссылка на элемент данных для удобства.
                Base*const& lpItem = *_pos.cur;
                // Проверяем тип элемента.
                switch ( lpItem->_type )
                {
                case Base::jPair: // Пара "Имя" : "Значение"
                    {
                        // Преобразование к Pair для удобства.
                        Pair*& lpPair = (Pair*&) lpItem;

                        // Записываем пару.
                        ofile << indent.c_str ( ) << L"\""
                            << xString::repl ( lpPair->_name, L"\"", L"\\\"" ).c_str ( )
                            << L"\" : \""
                            << xString::repl ( lpPair->_value, L"\"", L"\\\"" ).c_str ( )
                            << L"\"";
                        // Если в объекте ещё есть элементы.
                        if ( _pos.cur + 1 != end )
                            // Дописываем запятую-разделитель.
                            ofile << L",";
                    };
                    break;
                case  Base::jObject: // Объект.
                    {
                        // Преобразование к Object для удобства.
                        Object*& lpObject = (Object*&) lpItem;

                        // Записываем заголовок объекта.
                        ofile << indent.c_str ( )
                            << L"\""
                            << xString::repl ( lpObject->_name, L"\"", L"\\\"" ).c_str ( )
                            << L"\"" << std::endl
                            << indent.c_str ( ) << L"{" << std::endl;

                        // При возвращении к этому объекту необходимо
                        // начинать просмотр данных со следующей записи.
                        _pos.cur++;
                        // Помещаем текущий объект в стек.
                        _stack.push ( _pos );
                        // Изменяем текущий элемент стека.
                        _pos = stack_item ( lpObject );

                        // Переходим к обработке новго объекта.
                        goto l_write_object;
                    };
                    break;
                default: // Неопознанный элемент.
                    {
                        // Шта?
                    };
                };
                // Если это не последний элемент или стек ещё не пуст.
                if ( _pos.cur + 1 != end || !_stack.empty ( ) )
                    // Записываем перевод строки.
                    ofile << std::endl;
            };

            // Конец объекта. Проверяем стек.
            if ( !_stack.empty ( ) )
            {
                // Возвращаем последний элемент стека.
                _pos = _stack.top ( );
                // Удаляем его из стека.
                _stack.pop ( );

                // Форматирование для конечной точки объекта на одну табуляцию меньше.
                indent.erase ( 0, Settings.nIndent );
                // Записываем признак конца объекта.
                ofile << indent.c_str ( ) << L"}";
                // Если позиция записи объекта не достигла его конца.
                if ( _pos.cur != _pos.obj->_childs.end ( ) )
                {
                    // Если текущий обрабатываемый объект является корневым.
                    if ( _pos.obj == &lIn )
                        // Записываем перевод строки.
                        ofile << std::endl;
                    else
                        // Дописываем разделитель.
                        ofile << L",";
                };
                // Если стек ещё не пустой.
                if ( !_stack.empty ( ) )
                    // Записываем перевод строки.
                    ofile << std::endl;

                // Переходим к обработке объекта выдернутого из стека.
                goto l_write_object;
            };
        };

        // Типа Ок ;)
        bResult = true;
        // Закрываем файловый поток после завершения всех операций.
        ofile.close ( );
    };
    // Возвращаем результат.
    return bResult;
}; // write
