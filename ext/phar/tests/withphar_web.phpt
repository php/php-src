--TEST--
Phar: default web stub, with phar extension
--INI--
default_charset=UTF-8
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/withphar_web.php
REQUEST_URI=/withphar_web.php/web.php
PATH_INFO=/web.php
--FILE_EXTERNAL--
files/nophar.phar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECT--
web
