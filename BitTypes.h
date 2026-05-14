#if !defined(_Types_HEADER_BY_DMITRY_PYLILO_20091230)
#define _Types_HEADER_BY_DMITRY_PYLILO_20091230


/// Пространство имен утилитарных типов и функций
namespace u {
	// Элементарные типы
	typedef void*               ptr;    ///< Указатель
	typedef const void*         cptr;   ///< Константный указатель
	typedef unsigned int        size;   ///< Размер
	typedef int                 bul;    ///< Логический тип
	typedef unsigned char       uint8;  ///< Беззнаковый 1-но байтовый тип
	typedef unsigned short      uint16; ///< Беззнаковый 2-х байтовый тип
	typedef unsigned long       uint32; ///< Беззнаковый 4-х байтовый тип
	typedef unsigned long long  uint64; ///< Беззнаковый 8-ми байтовый тип
	typedef char                int8;   ///< Знаковый 1-но байтовый тип
	typedef short               int16;  ///< Знаковый 2-х байтовый тип
	typedef long                int32;  ///< Знаковый 4-х байтовый тип
	typedef long long           int64;  ///< Знаковый 8-ми байтовый тип

	/// Перечисление для логического типа
    enum {
		yes = 1, ///< Истина
		no = 0   ///< Ложь
	};

	// Абстрактные строковые типы
	typedef wchar_t             wchar;  ///< Wide символ
	typedef char                achar;  ///< Символ
	typedef wchar*              wstr;   ///< Wide строка
	typedef achar*              astr;   ///< Строка
	typedef const wchar*        wcstr;  ///< Константная Wide строка
	typedef const achar*        acstr;  ///< Константная строка

	// Сложные типы
    typedef struct {
		ptr data;   ///< Указатель на данные
		size dsize; ///< Размер данных в байтах
	} sdata; ///< Данные

    enum Crypt {
        Encrypt, ///< Шифрование
        Decrypt  ///< Расшифрование
    };
}


#endif // _Types_HEADER_BY_DMITRY_PYLILO_20091230
