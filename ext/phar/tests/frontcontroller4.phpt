--TEST--
Phar front controller index.php relocate (no /)
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller4.php
REQUEST_URI=/frontcontroller4.php
--FILE_EXTERNAL--
files/frontcontroller.phar
--EXPECTHEADERS--
Status: 301 Moved Permanently
Location: /frontcontroller4.php/index.php
--EXPECT--
