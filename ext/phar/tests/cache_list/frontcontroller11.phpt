--TEST--
Phar front controller mime type extension is not a string [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/frontcontroller11.php
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip phar extension not loaded"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller11.php
REQUEST_URI=/frontcontroller11.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
files/frontcontroller5.phar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECT--
hio
