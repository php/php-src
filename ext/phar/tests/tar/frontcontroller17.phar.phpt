--TEST--
Phar front controller mime type unknown tar-based
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller17.phar.php
REQUEST_URI=/frontcontroller17.phar.php/fronk.gronk
PATH_INFO=/fronk.gronk
--FILE_EXTERNAL--
files/frontcontroller8.phar.tar
--EXPECTHEADERS--
Content-Type: application/octet-stream
Content-Length: 4
--EXPECT--
hio3
