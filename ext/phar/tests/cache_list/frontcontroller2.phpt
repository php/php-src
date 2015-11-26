--TEST--
Phar front controller PHP test [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list=frontcontroller2.php
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller2.php
REQUEST_URI=/frontcontroller2.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
files/frontcontroller.phar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECT--
hio
