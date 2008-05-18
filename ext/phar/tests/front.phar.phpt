--TEST--
Phar front controller with mounted external file
--INI--
default_charset=
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/front.phar.php
REQUEST_URI=/front.phar.php/index.php
PATH_INFO=/index.php
--EXPECTHEADERS--
Content-type: text/html
--FILE_EXTERNAL--
files/blog.phar
--EXPECT--
string(0) ""