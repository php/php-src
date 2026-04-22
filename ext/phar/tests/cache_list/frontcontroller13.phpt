--TEST--
Phar front controller mime type not string/int [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/frontcontroller13.php
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller13.php
REQUEST_URI=/frontcontroller13.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
files/frontcontroller7.phar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECT--
TypeError: Phar::webPhar(): Argument #4 ($mimeTypes) mime type specifier must be of type string|int, null given
