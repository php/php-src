--TEST--
Phar front controller index.php relocate tar-based
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller5.phar.php
REQUEST_URI=/frontcontroller5.phar.php/
PATH_INFO=/
--FILE_EXTERNAL--
files/frontcontroller.phar.tar
--EXPECTHEADERS--
Status: 301 Moved Permanently
Location: /frontcontroller5.phar.php/index.php
--EXPECT--
