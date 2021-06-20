--TEST--
Phar front controller mime type unknown
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller17.php
REQUEST_URI=/frontcontroller17.php/fronk.gronk
PATH_INFO=/fronk.gronk
--FILE_EXTERNAL--
files/frontcontroller8.phar
--EXPECTHEADERS--
Content-type: application/octet-stream
Content-length: 4
--EXPECT--
hio3
