--TEST--
Phar front controller phps
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller5.php/
REQUEST_URI=/frontcontroller5.php/
--FILE_EXTERNAL--
frontcontroller.phar
--EXPECTHEADERS--
Status: 301 Moved Permanently
Location: /frontcontroller5.php/index.php
--EXPECT--
