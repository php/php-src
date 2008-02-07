--TEST--
Phar front controller include from cwd test 1
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller22.php
REQUEST_URI=/frontcontroller22.php/index.php
PATH_INFO=/index.php
--FILE_EXTERNAL--
frontcontroller13.phar
--EXPECTHEADERS--
Content-type: text/html
--EXPECTF--
string(4) "test"
string(12) "oof/test.php"

Warning: include(phar://%s/./hi.php): failed to open stream: phar error: "hi.php" is not a file in phar "%sfrontcontroller22.php" in phar://%s/oof/test.php on line %d

Warning: include(): Failed opening './hi.php' for inclusion (include_path='%s') in phar://%soof/test.php on line %d