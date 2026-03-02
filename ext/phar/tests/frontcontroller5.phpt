--TEST--
Phar front controller index.php relocate
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller5.php
REQUEST_URI=/frontcontroller5.php/
PATH_INFO=/
--FILE_EXTERNAL--
files/frontcontroller.phar
--EXPECTHEADERS--
Status: 301 Moved Permanently
Location: /frontcontroller5.php/index.php
--EXPECT--
