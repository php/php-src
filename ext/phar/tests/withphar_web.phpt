--TEST--
Phar: default web stub, with phar extension
--INI--
default_charset=
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--ENV--
SCRIPT_NAME=/withphar_web.php
REQUEST_URI=/withphar_web.php/web.php
PATH_INFO=/web.php
--FILE_EXTERNAL--
files/nophar.phar
--EXPECTHEADERS--
Content-type: text/html
--EXPECT--
web
