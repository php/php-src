--TEST--
Phar front controller other
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
detect_unicode=0
--ENV--
SCRIPT_NAME=/frontcontroller1.php
REQUEST_URI=/frontcontroller1.php/a.jpg
PATH_INFO=/a.jpg
--FILE_EXTERNAL--
files/frontcontroller.phar
--EXPECTHEADERS--
Content-type: image/jpeg
Content-length: 3
--EXPECT--
hio
