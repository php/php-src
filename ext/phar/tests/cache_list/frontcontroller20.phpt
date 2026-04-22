--TEST--
Phar front controller $_SERVER munging failure 3 [cache_list]
--INI--
phar.cache_list={PWD}/frontcontroller20.php
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller20.php
REQUEST_URI=/frontcontroller20.php/
PATH_INFO=/
--FILE_EXTERNAL--
files/frontcontroller11.phar
--EXPECT--
TypeError: Phar::mungServer(): Argument #1 ($variables) must be an array of strings, array given
