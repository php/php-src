--TEST--
Phar front controller mime type extension is not a string tar-based
--INI--
default_charset=
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller11.phar.php
REQUEST_URI=/frontcontroller11.phar.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
files/frontcontroller5.phar.tar
--EXPECTHEADERS--
Content-type: text/html
--EXPECT--
hio
