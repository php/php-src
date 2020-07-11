--TEST--
Phar front controller with fatal error in php file
--INI--
default_charset=UTF-8
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller29.php
REQUEST_URI=/frontcontroller29.php/fatalerror.phps
PATH_INFO=/fatalerror.phps
--FILE_EXTERNAL--
files/frontcontroller8.phar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function oopsie_daisy() in phar://%sfatalerror.phps:1
Stack trace:
#0 [internal function]: unknown()
#1 %s(%d): Phar::webPhar('whatever', 'index.php', '404.php', Array)
#2 {main}
  thrown in phar://%sfatalerror.phps on line 1
