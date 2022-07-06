--TEST--
Phar front controller alternate index file [cache_list]
--INI--
phar.cache_list={PWD}/frontcontroller7.php
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
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
