--TEST--
Phar front controller $_SERVER munging failure [cache_list]
--INI--
phar.cache_list={PWD}/frontcontroller18.php
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller18.php
REQUEST_URI=/frontcontroller18.php/fronk.gronk
PATH_INFO=/fronk.gronk
--FILE_EXTERNAL--
files/frontcontroller9.phar
--EXPECT--
ValueError: Phar::mungServer(): Argument #1 ($variables) must not be empty
