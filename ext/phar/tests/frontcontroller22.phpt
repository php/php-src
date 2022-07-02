--TEST--
Phar front controller include from cwd test 1
--INI--
default_charset=UTF-8
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller22.php
REQUEST_URI=/frontcontroller22.php/index.php
PATH_INFO=/index.php
--FILE_EXTERNAL--
files/frontcontroller13.phar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECTF--
string(4) "test"
string(12) "oof/test.php"

Warning: include(./hi.php): Failed to open stream: No such file or directory in phar://%s/oof/test.php on line %d

Warning: include(): Failed opening './hi.php' for inclusion (include_path='%s') in phar://%soof/test.php on line %d
