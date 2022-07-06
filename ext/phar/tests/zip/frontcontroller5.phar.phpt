--TEST--
Phar front controller index.php relocate zip-based
--INI--
phar.require_hash=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller5.phar.php
REQUEST_URI=/frontcontroller5.phar.php/
PATH_INFO=/
--FILE_EXTERNAL--
files/frontcontroller.phar.zip
--EXPECTHEADERS--
Status: 301 Moved Permanently
Location: /frontcontroller5.phar.php/index.php
--EXPECT--
