--TEST--
Phar front controller with valid callback that is not good [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/frontcontroller32.php
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller32.php
REQUEST_URI=/frontcontroller32.php
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--FILE_EXTERNAL--
files/frontcontroller17.phar
--EXPECTF--
%ahar error: rewrite callback must return a string or false
