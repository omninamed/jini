//////////////////////////////////////////////////////////////////////////
// jini - json-подобный формат файлов инициализации.
//
// Разработчик: Омни
// Дата:        23.05.2016
// Кодировка:   UNICODE
// Версия:      0.3 build 27
//
// Особенности версии:
// - Проект переориентирован на "свой" формат позиционируемый как упрощённый вариант json
//   Собственно тот же json только без массивов.
// - Отвязка от ATL, теперь всё на чистом STL
// - Добавлена структура настроек движка.
// - Добавлены списки синонимов для логических значений.
// - Переработан доступ к элементам дерева по пути.
// - Буфер чтения внутри анализатора более не изменяется на каждом этапе.
// - Исправлены баги анализатора при чтении синтаксически не верных файлов, скорее всего не все баги... ;)
// - Отсутствует обработка ошибок синтаксиса в анализаторе. Все ошибки просто игнорируются.
//
// Особенности движка:
// - Линейная структура - порядок элементов соответствует порядку в файле.
// - Простое и безопасное преобразование значений пар.
// - Теоретически, любая глубина вложений.
// - Читает ВСЁ что может прочитать игнорируя ВСЕ ошибки синтаксиса.
// - Существующие объекты и пары дополняются/обновляются данными из файла (синхронизирующее чтение)
// - При нахождении элемента с дублирующимся именем, но не совпадающим типом пропускает
//   чтение всего элемента (включая вложения, если это объект)

#pragma once

// std::locale
#include <locale>
// std::wstring
#include <string>
// std::vector
#include <vector>
// Стандартные алгоритмы.
#include <algorithm>

// Операции со строками.
namespace xString
{
    // Переопределение вектора строк.
    typedef std::vector < std::wstring > wsvector;

    // Сравнивает две строки без учёта регистра используя текущую локаль.
    inline bool iequal ( const std::wstring& s1, const std::wstring& s2 )
    {
        return ( s1.size ( ) == s2.size ( ) ) && std::equal ( s1.begin ( ), s1.end ( ), s2.begin ( ),
            [] ( const wchar_t& ch1, const wchar_t& ch2 ) { return ::_towlower_l ( ch1, nullptr ) == ::_towlower_l ( ch2, nullptr ); } );
    }; // iequal

    // Замена всех подстрок.
    inline std::wstring repl ( std::wstring str, const std::wstring substr, const std::wstring replstr )
    {
        for ( std::size_t i = 0; ( i = str.find ( substr, i ) ) != std::wstring::npos; i += replstr.size ( ) )
            str.replace ( i, substr.size ( ), replstr );
        return str;
    }; // Replace

    // Разделение строки.
    inline void split ( const std::wstring& s, const std::wstring& delim, wsvector& v )
    {
        if ( delim.empty ( ) )
            v.push_back ( s );
        else
        {
            std::wstring::const_iterator sub = s.cbegin ( );
            for (;;)
            {
                std::wstring::const_iterator end =
                    std::search ( sub, s.cend ( ), delim.cbegin ( ), delim.cend ( ) );
                v.push_back ( std::wstring ( sub, end ) );
                if ( end == s.end ( ) )
                    break;
                sub = end + delim.size ( );
            };
        };
    }; // split
}; // xString

// Логические значения.
namespace xBool
{
    // Пара синонимов для bool
    class Pair
    {
    public:

        // Синоним для true
        std::wstring _true;
        // Синоним для false
        std::wstring _false;

        // Конструктор по умолчанию.
        Pair ( const wchar_t* s_true, const wchar_t* s_false );

        // Возвращает синоним логического значения.
        const wchar_t* get_synonym ( bool value ) const;
        // Возвращает значение синонима, если он относится к текущей паре.
        bool get_value ( const wchar_t* synonym, bool& value ) const;
    }; // Pair

    // Энумерация пар синонимов логических значений.
    enum
    {
        // 1 и 0
        b_1_0,
        // true и false
        b_t_f,
        // yes и no
        b_y_n,
        // on и off
        b_o_o,
        // enabled и disabled
        b_e_d
    };

    // Список синонимов для bool
    class Synonyms
    {
    public:

        // Пара синонимов с идентификатором.
        struct PairEx : public Pair
        {
            // Идентификатор пары.
            unsigned short _id;

            // Конструктор по умолчанию.
            PairEx ( unsigned short id, const wchar_t* s_true, const wchar_t* s_false );
        }; // PairEx

        // Список синонимов для bool
        std::vector < PairEx > _synonyms;
    
        // Конструктор по умолчанию.
        Synonyms ( bool def = true );
        // Деструктор.
        ~Synonyms ( void );

        // Добавляет пару синонимов в список.
        Pair* add_pair ( unsigned short id, const wchar_t* s_true, const wchar_t* s_false );

        // Возвращает указатель на пару синонимов.
        Pair* get_pair ( unsigned short id ) const;
        // Возвращает указатель на пару синонимов.
        Pair* get_pair ( const wchar_t* synonym ) const;

        // Удаляет пару из списка.
        void erase ( unsigned short id );
        // Удаляет все пары из списка.
        void erase ( void );
    }; // Synonyms
}; // xBool

// jini
namespace jini
{
    // Структура настроек движка.
    struct tagSettings
    {
        // Пропуск пробелов в начале многострочных литералов.
        bool bSkipWhitespaces;  // По умолчанию false
        // Размер отступа для вложенных объектов.
        unsigned short nIndent; // По умолчанию 2

        // Список пар синонимов для jini
        xBool::Synonyms xBooleans; // По умолчанию базовый список.

        // Конструктор по умолчанию.
        tagSettings ( void );
    }; // tagSettings

    // Настройки движка.
    extern tagSettings Settings;

    //////////////////////////////////////////////////////////////////////////

    // Базовый класс для любых объектов json | jini
    class Base
    {
    public:

        // Указатель на родительский объект.
        Base* _parent;

        enum
        {
            // Пара ["Имя":"Значение"]
            jPair = 1,
            // Объект ["Имя"{...}]
            jObject
        };

        // Тип.
        unsigned short _type;

        // Имя.
        std::wstring _name;
    
        // Конструктор по умолчанию.
        Base ( unsigned short type, const wchar_t* name, Base* parent = nullptr );
        // Конструктор копирования.
        Base ( const Base& source, Base* parent = nullptr );
        // Деструктор.
        virtual ~Base ( void );

        // Создаёт точную копию текущего объекта.
        virtual Base* clone ( Base* parent = nullptr ) const = 0;

        // Возвращает корневой элемент.
        Base* get_root ( void ) const;
    }; // Base

    // Представляет пару ["Имя":"Значение"]
    class Pair : public Base
    {
    public:

        // RAW-значение.
        std::wstring _value;
    
        // Конструктор по умолчанию.
        Pair ( const wchar_t* name = L"", const wchar_t* value = L"", Base* parent = nullptr );
        // Конструктор копирования.
        Pair ( const Pair& source, Base* parent = nullptr );
        // Деструктор.
        ~Pair ( void );

        // Создаёт точную копию текущего объекта.
        virtual Base* clone ( Base* parent = nullptr ) const;

        // Форматирование значения.
        void format ( const wchar_t* fmt, ... );

        // Устанавливает строковое значение.
        void set_string ( const wchar_t* value );
        // Устанавливает логическое значение.
        void set_bool ( bool value, unsigned short pair_id = xBool::b_1_0 );
        // Устанавливает целочисленное значение.
        void set_int ( int value, int radix = 10 );
        // Устанавливает значение с плавающей точкой.
        void set_float ( float value, int precision = 3 );

        // Возвращает строковое значение.
        const wchar_t* get_string ( const wchar_t* def = L"" ) const;
        // Возвращает логическое значение.
        bool get_bool ( bool def = false ) const;
        // Возвращает целочисленное значение.
        int get_int ( int def = 0, int radix = 0 ) const;
        // Возвращает значение с плавающей точкой.
        float get_float ( float def = 0.0f ) const;
    }; // Pair

    // Представляет объект ["Имя"{...}]
    class Object : public Base
    {
    public:

        // Массив дочерних элементов.
        std::vector < Base* > _childs;
    
        // Конструктор по умолчанию.
        Object ( const wchar_t* name = L"", Base* parent = nullptr );
        // Конструктор по умолчанию.
        Object ( const Object& source, Base* parent = nullptr );
        // Деструктор.
        ~Object ( void );

        // Создаёт точную копию текущего объекта.
        virtual Base* clone ( Base* parent = nullptr ) const;

        // Добавляет пару.
        Pair* add_pair ( const wchar_t* name, const wchar_t* value );
        // Добавляет объект.
        Object* add_object ( const wchar_t* name );

        // Возвращает элемент без проверки типа.
        Base* get_element ( const wchar_t* n_o_p, const wchar_t* delim = nullptr ) const;
        // Возвращает пару.
        Pair* get_pair ( const wchar_t* n_o_p, const wchar_t* delim = nullptr ) const;
        // Возвращает объект.
        Object* get_object ( const wchar_t* n_o_p, const wchar_t* delim = nullptr ) const;

        // Проверяет, есть ли элемент в текущем объекте.
        bool is_exist ( const wchar_t* name ) const;

        // Удаляет дочерний элемент.
        void erase ( const wchar_t* name );
        // Удаляет все дочерние элементы.
        void erase ( void );
    }; // Object

    // Считывает файл .jini
    bool read ( Object& lOut, const std::locale& loc );
    // Записывает файл .jini
    bool write ( const Object& lIn, const std::locale& loc );
}; // jini
