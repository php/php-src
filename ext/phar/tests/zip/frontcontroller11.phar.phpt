--TEST--
Phar front controller mime type extension is not a string zip-based
--INI--
default_charset=
phar.require_hash=0
--EXTENSIONS--
phar
zlib
--ENV--
SCRIPT_NAME=/frontcontroller11.phar.php
REQUEST_URI=/frontcontroller11.phar.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
files/frontcontroller5.phar.zip
--EXPECTHEADERS--
Content-type: text/html
--EXPECT--
hio
