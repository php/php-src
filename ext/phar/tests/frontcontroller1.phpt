--TEST--
Phar front controller other
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller1.php
REQUEST_URI=/frontcontroller1.php/a.jpg
PATH_INFO=/a.jpg
--FILE_EXTERNAL--
files/frontcontroller.phar
--EXPECTHEADERS--
Content-Type: image/jpeg
Content-Length: 3
--EXPECT--
hio
