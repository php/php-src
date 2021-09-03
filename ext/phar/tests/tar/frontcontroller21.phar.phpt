--TEST--
Phar front controller $_SERVER munging success tar-based
--INI--
default_charset=UTF-8
cgi.fix_pathinfo=1
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller21.phar.php
REQUEST_URI=/frontcontroller21.phar.php/index.php?test=hi
PATH_INFO=/index.php
QUERY_STRING=test=hi
--FILE_EXTERNAL--
files/frontcontroller12.phar.tar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECTF--
string(10) "/index.php"
string(10) "/index.php"
string(%d) "phar://%sfrontcontroller21.phar.php/index.php"
string(18) "/index.php?test=hi"
string(37) "/frontcontroller21.phar.php/index.php"
string(27) "/frontcontroller21.phar.php"
string(%d) "%sfrontcontroller21.phar.php"
string(45) "/frontcontroller21.phar.php/index.php?test=hi"
