--TEST--
Phar front controller with invalid callback for rewrites [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/frontcontroller31.php
--EXTENSIONS--
phar
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die('xleak LSan crashes for this test');
?>
--ENV--
SCRIPT_NAME=/frontcontroller31.php
REQUEST_URI=/frontcontroller31.php
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--FILE_EXTERNAL--
files/frontcontroller16.phar
--EXPECT--
TypeError: Phar::webPhar(): Argument #5 ($rewrite) must be a valid callback or null, class "fail" not found
