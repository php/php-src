--TEST--
Phar front controller mime type override, other zip-based
--INI--
phar.require_hash=0
--EXTENSIONS--
phar
zlib
--ENV--
SCRIPT_NAME=/frontcontroller14.phar.php
REQUEST_URI=/frontcontroller14.phar.php/a.jpg
PATH_INFO=/a.jpg
--FILE_EXTERNAL--
files/frontcontroller8.phar.zip
--EXPECTHEADERS--
Content-type: foo/bar
Content-length: 4
--EXPECT--
hio2
