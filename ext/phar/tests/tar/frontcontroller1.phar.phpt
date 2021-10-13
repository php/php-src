--TEST--
Phar front controller other tar-based
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller1.phar.php
REQUEST_URI=/frontcontroller1.phar.php/a.jpg
PATH_INFO=/a.jpg
--FILE_EXTERNAL--
files/frontcontroller.phar.tar
--EXPECTHEADERS--
Content-Type: image/jpeg
Content-Length: 3
--EXPECT--
hio
