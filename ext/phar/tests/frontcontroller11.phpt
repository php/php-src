--TEST--
Phar front controller mime type extension is not a string
--INI--
default_charset=
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip phar extension not loaded"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller11.php
REQUEST_URI=/frontcontroller11.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
files/frontcontroller5.phar
--EXPECTHEADERS--
Content-type: text/html
--EXPECT--
hio
