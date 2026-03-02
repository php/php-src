--TEST--
Phar front controller with cwd [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/frontcontroller34.php
--EXTENSIONS--
phar
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die('xleak LSan crashes for this test');
?>
--ENV--
SCRIPT_NAME=/frontcontroller34.php
REQUEST_URI=/frontcontroller34.php/start/index.php
PATH_INFO=/start/index.php
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--FILE_EXTERNAL--
files/frontcontroller19.phar
--EXPECT--
start/index.php
start/another.php
another.php
