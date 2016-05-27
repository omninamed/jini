//////////////////////////////////////////////////////////////////////////
// Реализация метода jini::Read

// jini
#include "jini.h"

// std::wifstream и std::wofstream
#include <fstream>

// Использовать пространство имён jini глобально для текущего файла.
using namespace jini;

// Считывает файл .jini
bool jini::read ( Object& lOut, const std::locale& loc )
{
    // Результат метода.
    bool bResult = false;
    // Открываем файл в конструкторе.
    std::wifstream ifile ( lOut._name, std::ios::in );
    // Открыт?
    if ( ifile.is_open ( ) )
    {
        // Локаль для файла.
        ifile.imbue ( loc );

        std::wstring
            // Буфер чтения.
            buf,
            // Временный буфер для многострочных литералов.
            multiline,
            // Буфер для имени считанного летарала.
            name,
            // Буфер для значения пары.
            value;

        int
            // Текущая строка.
            nLine = 1,
            // Текущий столбец.
            nColumn,
            // Общее количество столбцов (символов в текущей строке)
            nColumnCount,
            // Глубина вложенности пропускаемых объектов.
            nSkipDepth = 0;

        bool
            // Признак многострочного литерала.
            bIsMultiline = false,
            // Признак считанного имени литерала.
            bIsNamed = false,
            // Признак считанного разделителя пары.
            bIsPair = false;

        // Текущий обрабатываемый объект.
        Object* pCurrent = &lOut;

        // Читаем до победного!
        while ( ifile.good ( ) )
        {
            // Выдёргиваем строку из файла.
            std::getline ( ifile, buf );
            // Получаем буфер.
            const wchar_t* pBuffer = buf.c_str ( );
            // Начальный столбец.
            nColumn = 0;
            // Всего столбцов.
            nColumnCount = buf.size ( );
            // Посимвольный анализ считанной строки.
            while ( nColumn < nColumnCount )
            {
                // Ссылка для удобства.
                const wchar_t& ch = pBuffer [ nColumn ];
                // Ранее был считан не завершённый литерал?
                if ( bIsMultiline && !Settings.bSkipWhitespaces )
                    // Переходим к последующей обработке, конечно если в
                    // настройках не указан пропуск пробелов в начале строк.
                    goto l_process_literal;

                // Пропуск пробелов слева строки:

                if ( std::isspace < wchar_t > ( ch, loc ) )
                {
                    nColumn++;
                    continue;
                };

                // Что тут у нас?
                switch ( ch )
                {
                case L'\"': // Начало литерала.
                    {
l_process_literal:
                        // Начальный индекс для поиска закрытия литерала.
                        int nFindex = bIsMultiline ? nColumn : ++nColumn;
                        // Поиск закрывающего символа.
                        while ( nFindex < nColumnCount )
                        {
                            // Кавычка? Она первая или не экранирована?
                            if ( pBuffer [ nFindex ] == L'\"' && ( nFindex - 1 < 0 || pBuffer [ nFindex - 1 ] != L'\\' ) )
                                // Конец литерала найден, прерываем цикл поиска.
                                break;
                            nFindex++; // Плюс индекс поиска.
                        };

                        // Сохраняем флаг многострочного литерала.
                        bool old_bIsMultiline = bIsMultiline;
                        // Обновляем флаг.
                        bIsMultiline = nFindex == nColumnCount;
                        // Литерал не завершён?
                        if ( bIsMultiline )
                            // Присоединяем к соответствующему буферу.
                            multiline.append ( pBuffer + nColumn );
                        else
                        {
                            // Ссылка на буфер для литерала в зависимости от состояния анализатора.
                            std::wstring& literal_link = bIsPair ? value : name;
                            // Буфер не пусто? wtf?
                            if ( !literal_link.empty ( ) )
                            {
                                // Ошибка синтаксиса - несколько литералов подряд.

                                // Ранее был считан разделитель пары.
                                if ( bIsPair )
                                {
                                    // Ошибка при считывании значения пары.
                                };
                                // Очищаем буфер.
                                multiline.erase ( );
                                // Переходим к метке пропуска ошибки.
                                goto l_skip_literla_error;
                            }
                            else if ( old_bIsMultiline ) // Означает что это конец многострочного литерала.
                            {
                                // Присоединяем буфер.
                                literal_link.append ( multiline );
                                // Очищаем специальный буфер.
                                multiline.erase ( );
                            };
                            // Добавляем последнюю строку.
                            literal_link.append ( pBuffer + nColumn, nFindex - nColumn );
                            // Ранее был считан разделитель пары?
                            if ( bIsPair )
                            {
                                // Без имени!
                                if ( !bIsNamed )
                                {
                                    // Ошибка синтаксиса - безымянная пара.
                                    // Т.е. имя не пустое, а вообще отсутствует.
                                    // Unexpected pair value так сказать.
                                    // Оставим без пропуска, будет безымянная пара.
                                };
                                // Обработка пары только если она не принадлежит к пропускаемому объекту.
                                if ( !nSkipDepth )
                                {
                                    // Пытаемся получить элемент.
                                    Base* pElement = pCurrent->get_element ( name.c_str ( ) );
                                    // Есть?
                                    if ( pElement )
                                    {
                                        // Дубликат.

                                        // Проверяем тип элемента.
                                        switch ( pElement->_type )
                                        {
                                        case Base::jPair: // Пара.
                                            // Обновляем значение.
                                            ((Pair*)pElement)->_value = value.c_str ( );
                                            break;
                                        case Base::jObject: // Объект.
                                        default: // Шта?
                                            {
                                                // Ошибка - имя считанного и исходного элементов совпадают,
                                                // но исходный элемент не является парой.
                                            };
                                        };
                                    }
                                    else
                                        // Добавляем новыую пару в текущий обрабатываемый объект.
                                        pCurrent->_childs.push_back ( new Pair ( name.c_str ( ), value.c_str ( ), pCurrent ) );
                                };
                                // Очищаем буферы.
                                name.erase ( ); value.erase ( );
                                // Сбрасываем флаги.
                                bIsNamed = false; bIsPair = false;
                            }
                            else if ( bIsNamed )
                            {
                                // Шта?
                                // Сюда по идее вообще никогда не должно
                                // попадать из-за механики l_skip_literla_error
                            }
                            else
                                // Считан заголовок.
                                bIsNamed = true;
                        };
l_skip_literla_error:
                        // Обновляем значение столбца для дальнейшего анализа.
                        nColumn = nFindex + 1;
                        // Следующая итерация, т.к. все данные уже обработаны.
                        continue;
                    };
                    break;
                case L':': // Разделитель пары.
                    {
                        if ( nSkipDepth )
                        {
                            // Пропуск.
                        };
                        if ( bIsPair )
                        {
                            // Ошибка - лишний разделитель ':'
                        };
                        if ( !bIsNamed )
                        {
                            // Ошибка - у пары нет имени.
                        };
                        bIsPair = true;
                    };
                    break;
                case L',': // Формальный индикатор продолжения списка.
                    break;
                case L'{': // Начало объекта.
                    {
                        // Пропуск дубликата с несопоставимым типом.
                        if ( nSkipDepth )
                        {
                            // Плюс глубина вложенности.
                            nSkipDepth++;
                            // Переходим к концу обработчика.
                            goto l_skip_this_object;
                        };

                        if ( bIsPair )
                        {
                            // Ошибка синтаксиса - '{' после ':'

                            // Сбрасываем флаг пары и будем надеяться что это действительно объект ;)
                            bIsPair = false;
                        };
                        if ( !bIsNamed )
                        {
                            // Отсутствует имя.

                            // Это не обязательно ошибка, безымянные объекты допустимы.
                        };

                        // Проверяем, есть ли такой объект в списке.
                        Base* pElement = pCurrent->get_element ( name.c_str ( ) );
                        // Есть?
                        if ( pElement )
                        {
                            // Дубликат.

                            // Проверяем тип.
                            switch ( pElement->_type )
                            {
                            case Base::jObject: // Объект.
                                // Копируем указатель для обновления/добавления данных.
                                pCurrent = (Object*) pElement;
                                break;
                            default:
                                {
                                    // Ошибка - имя считанного и исходного элементов совпадают,
                                    // но исходный элемент не является объектом.

                                    // Плюсуем глубину вложенности для дальнейшего пропуска всего объекта.
                                    nSkipDepth++;
                                    // Плюс столбец.
                                    nColumn++;
                                    // Следующая итерация прасера.
                                    continue;
                                };
                            };
                        }
                        else
                        {
                            // Новый объект.

                            // Копируем указатель на текущий объект, он будет родительским.
                            Object* pParent = pCurrent;
                            // Создаём новый объект.
                            pCurrent = new Object ( name.c_str ( ), pParent );
                            // Добавляем его в список.
                            pParent->_childs.push_back ( pCurrent );
                        };
l_skip_this_object:
                        // Очищаем имя.
                        name.erase ( );
                        // Сбрасываем флаг наличия имени.
                        bIsNamed = false;
                    };
                    break;
                case L'}': // Конец объекта.
                    {
                        // Если ранее был инициирован пропуск объекта.
                        if ( nSkipDepth )
                            // Уменьшаем глубину вложенности.
                            nSkipDepth--;
                        else if ( pCurrent->_parent )
                            pCurrent = (Object*) pCurrent->_parent;
                        else
                        {
                            // Ошибка, все объекты уже закрыты.
                        };
                    };
                    break;
                default: // Неопознанный синтаксический элемент.
                    {
                        // Игнорируем ошибку. Хотя не стоило бы ;)
                    };
                };
                // Плюсуем счётчик столбцов.
                nColumn++;
            };
            // Если чтение файла всё ещё не завершено.
            if ( ifile.good ( ) )
                // Плюсуем счётчик строк.
                nLine++;
        };
        // Закрываем файловый поток.
        ifile.close ( );
        // Ок, если считано хоть что-то.
        bResult = nLine > 1;
    };
    // Возвращаем результат.
    return bResult;
}; // read
