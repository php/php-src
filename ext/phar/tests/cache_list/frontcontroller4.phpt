--TEST--
Phar front controller index.php relocate (no /) [cache_list]
--INI--
phar.cache_list={PWD}/frontcontroller4.php
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller4.php
REQUEST_URI=/frontcontroller4.php
--FILE_EXTERNAL--
files/frontcontroller.phar
--EXPECTHEADERS--
Status: 301 Moved Permanently
Location: /frontcontroller4.php/index.php
--EXPECT--
