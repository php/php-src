--TEST--
Phar front controller with valid callback that is not good
--INI--
default_charset=UTF-8
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller32.php
REQUEST_URI=/frontcontroller32.php
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--FILE_EXTERNAL--
files/frontcontroller17.phar
--EXPECT--
TypeError: Rewrite callback return value must be of type string|false, null returned
