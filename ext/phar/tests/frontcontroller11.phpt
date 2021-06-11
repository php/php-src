--TEST--
Phar front controller mime type extension is not a string
--INI--
default_charset=
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller11.php
REQUEST_URI=/frontcontroller11.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
files/frontcontroller5.phar
--EXPECTHEADERS--
Content-type: text/html
--EXPECT--
hio
