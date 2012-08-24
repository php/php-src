--TEST--
Phar front controller alternate index file
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
detect_unicode=0
--ENV--
SCRIPT_NAME=/frontcontroller7.php
REQUEST_URI=/frontcontroller7.php/
PATH_INFO=/
--FILE_EXTERNAL--
files/frontcontroller2.phar
--EXPECTHEADERS--
Status: 301 Moved Permanently
Location: /frontcontroller7.php/a.php
--EXPECT--
