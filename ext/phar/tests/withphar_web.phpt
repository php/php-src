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
--EXPECTF--
Deprecated: class_exists(): Passing int to parameter #2 ($autoload) of type bool is deprecated in %s on line %d
web
