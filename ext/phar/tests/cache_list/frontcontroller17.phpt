--TEST--
Phar front controller mime type unknown [cache_list]
--INI--
phar.cache_list={PWD}/frontcontroller17.php
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller17.php
REQUEST_URI=/frontcontroller17.php/fronk.gronk
PATH_INFO=/fronk.gronk
--FILE_EXTERNAL--
files/frontcontroller8.phar
--EXPECTHEADERS--
Content-type: application/octet-stream
Content-length: 4
--EXPECT--
hio3

