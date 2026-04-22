--TEST--
Phar front controller with valid callback that is not good [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/frontcontroller32.php
--EXTENSIONS--
phar
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die('xleak LSan crashes for this test');
?>
--ENV--
SCRIPT_NAME=/frontcontroller32.php
REQUEST_URI=/frontcontroller32.php
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--FILE_EXTERNAL--
files/frontcontroller17.phar
--EXPECT--
TypeError: Rewrite callback return value must be of type string|false, null returned
