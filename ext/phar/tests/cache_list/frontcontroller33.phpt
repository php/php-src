--TEST--
Phar front controller with valid callback that does not return any value [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/frontcontroller33.php
--EXTENSIONS--
phar
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die('xleak LSan crashes for this test');
?>
--ENV--
SCRIPT_NAME=/frontcontroller33.php
REQUEST_URI=/frontcontroller33.php
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--FILE_EXTERNAL--
files/frontcontroller18.phar
--EXPECT--
phar error: rewrite callback must return a string or false
