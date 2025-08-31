--TEST--
Phar front controller mime type override, Phar::PHP zip-based
--INI--
default_charset=UTF-8
phar.require_hash=0
--EXTENSIONS--
phar
zlib
--ENV--
SCRIPT_NAME=/frontcontroller16.phar.php
REQUEST_URI=/frontcontroller16.phar.php/a.phps
PATH_INFO=/a.phps
--FILE_EXTERNAL--
files/frontcontroller8.phar.zip
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECT--
hio1
