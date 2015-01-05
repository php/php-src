--TEST--
Phar front controller mime type override, other tar-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller14.phar.php
REQUEST_URI=/frontcontroller14.phar.php/a.jpg
PATH_INFO=/a.jpg
--FILE_EXTERNAL--
files/frontcontroller8.phar.tar
--EXPECTHEADERS--
Content-type: foo/bar
Content-length: 4
--EXPECT--
hio2
