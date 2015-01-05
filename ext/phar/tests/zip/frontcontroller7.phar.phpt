--TEST--
Phar front controller alternate index file zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller7.phar.php
REQUEST_URI=/frontcontroller7.phar.php/
PATH_INFO=/
--FILE_EXTERNAL--
files/frontcontroller2.phar.zip
--EXPECTHEADERS--
Status: 301 Moved Permanently
Location: /frontcontroller7.phar.php/a.php
--EXPECT--
