--TEST--
Phar front controller alternate index file tar-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller7.phar.php
REQUEST_URI=/frontcontroller7.phar.php/
PATH_INFO=/
--FILE_EXTERNAL--
files/frontcontroller2.phar.tar
--EXPECTHEADERS--
Status: 301 Moved Permanently
Location: /frontcontroller7.phar.php/a.php
--EXPECT--
