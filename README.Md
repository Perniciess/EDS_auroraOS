# Описание
Проект для подписания электронных документов (ЭЦП) при помощи OpenSSL для дальнейшего внерения в приложение документооборота под OC Aurora.

Подразумевается, что на устройстве заранее имеется публичный и приватный ключ. 
Но было принято решение добавить функцию для генерации ключей дополнительно.


# Сборка проекта

```
make -f Makefile
```


Во время разработки использовались команды, можно использовать для ручной проверки работы openssl:
  1. ```openssl genrsa -out ./keys/privateKey.pem 2048``` - для генерации приватного ключа. Важно PEM > 4.
  2. ```openssl rsa -in key.pem -pubout -out ./keys/publicKey.pem``` - для извлечения публичного ключа
  3. ```openssl dgst -sha256 -out test.hash.txt test.txt ``` - генерация хэша файла
  4. ```openssl dgst -sha256 -sign key.pem -out hash.sig test.hash.txt``` - подпись хэша с помощью приватного ключа
  5. ```openssl dgst -sha256 -verify publickey.pem -signature hash.sig hash.txt ``` - проверка подлинности хэша


# Электронная цифровая подпись (EDS) - Структура проекта
```
EDS_cpp/
│
│
├── build                           # Собранный проект
│   └── CryptoKeyManager            # Исполняемый файл проекта **
│
│
├── include                         # Заголовочные файлы
│   └──CryptoKeyManager.hpp         # Интерфейс класса CryptoKeyManager
│
│
├── src/                            # Исходный код
│   ├── main.cpp                    # Точка входа
│   └── CryptoKeyManager.cpp        # Реализация методов класса CryptoKeyManager 
|
│
├── test_files/   
│   └─── test.pdf                   # Pdf файл для хэширования и шифрования
|
│
├── LICENSE                         # Лицензия
├── Makefile                        # Файл сборки
└── README.md                       # Описание проекта
```

** Генерируются при запуске исполняемого файла.


