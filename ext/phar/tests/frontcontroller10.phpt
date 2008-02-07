--TEST--
Phar front controller rewrite array invalid
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller10.php
REQUEST_URI=/frontcontroller10.php/hi
PATH_INFO=/hi
--FILE_EXTERNAL--
frontcontroller4.phar
--EXPECTHEADERS--
Content-type: text/html
--EXPECTF--
Fatal error: Uncaught exception 'UnexpectedValueException' with message 'phar rewrite value for "/hi" was not a string' in %sfrontcontroller10.php:2
Stack trace:
#0 %sfrontcontroller10.php(2): Phar::webPhar('whatever', 'index.php', '', Array, Array)
#1 {main}
  thrown in %sfrontcontroller10.php on line 2