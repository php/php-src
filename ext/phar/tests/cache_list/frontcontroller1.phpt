--TEST--
Phar front controller other
--INI--
phar.cache_list={PWD}/frontcontroller1.php [cache_list]
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller1.php
REQUEST_URI=/frontcontroller1.php/a.jpg
PATH_INFO=/a.jpg
--FILE_EXTERNAL--
files/frontcontroller.phar
--EXPECTHEADERS--
Content-type: image/jpeg
Content-length: 3
--EXPECT--
hio
