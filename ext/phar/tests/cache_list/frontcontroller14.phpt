--TEST--
Phar front controller mime type override, other [cache_list]
--INI--
phar.cache_list={PWD}/frontcontroller14.php
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller14.php
REQUEST_URI=/frontcontroller14.php/a.jpg
PATH_INFO=/a.jpg
--FILE_EXTERNAL--
files/frontcontroller8.phar
--EXPECTHEADERS--
Content-type: foo/bar
Content-length: 4
--EXPECT--
hio2
