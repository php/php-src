--TEST--
Phar: default web stub, with phar extension
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--ENV--
SCRIPT_NAME=/withphar_web.php/web.php
REQUEST_URI=/withphar_web.php/web.php
--FILE_EXTERNAL--
nophar.phar
--EXPECTHEADERS--
Content-type: text/html
--EXPECT--
web
