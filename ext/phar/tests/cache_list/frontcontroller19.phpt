--TEST--
Phar front controller $_SERVER munging failure 2 [cache_list]
--INI--
phar.cache_list={PWD}/frontcontroller19.php
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller19.php
REQUEST_URI=/frontcontroller19.php/
PATH_INFO=/
--FILE_EXTERNAL--
files/frontcontroller10.phar
--EXPECT--
ValueError: Phar::mungServer(): Argument #1 ($variables) must have at most 4 elements
