--TEST--
Phar front controller include from cwd test 1
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller23.php
REQUEST_URI=/frontcontroller23.php/hi/there
PATH_INFO=/hi/there
--FILE_EXTERNAL--
files/frontcontroller14.phar
--EXPECTHEADERS--
Content-type: text/html
--EXPECTF--
