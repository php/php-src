--TEST--
Phar front controller index.php relocate (no /) zip-based
--INI--
phar.require_hash=0
--EXTENSIONS--
phar
zlib
--ENV--
SCRIPT_NAME=/frontcontroller4.phar.php
REQUEST_URI=/frontcontroller4.phar.php
--FILE_EXTERNAL--
files/frontcontroller.phar.zip
--EXPECTHEADERS--
Status: 301 Moved Permanently
Location: /frontcontroller4.phar.php/index.php
--EXPECT--
