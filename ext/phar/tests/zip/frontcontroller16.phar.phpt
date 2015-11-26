--TEST--
Phar front controller mime type override, Phar::PHP zip-based
--INI--
default_charset=UTF-8
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller16.phar.php
REQUEST_URI=/frontcontroller16.phar.php/a.phps
PATH_INFO=/a.phps
--FILE_EXTERNAL--
files/frontcontroller8.phar.zip
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECT--
hio1

