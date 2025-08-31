--TEST--
Phar front controller index.php relocate (no /) tar-based
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller4.phar.php
REQUEST_URI=/frontcontroller4.phar.php
--FILE_EXTERNAL--
files/frontcontroller.phar.tar
--EXPECTHEADERS--
Status: 301 Moved Permanently
Location: /frontcontroller4.phar.php/index.php
--EXPECT--
