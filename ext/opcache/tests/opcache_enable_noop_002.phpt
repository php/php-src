--TEST--
Dynamically setting opcache.enable warns when not a noop
--INI--
opcache.enable=0
opcache.enable_cli=1
--EXTENSIONS--
opcache
--FILE--
<?php

echo "Should warn, since the INI was initialized to 0:", PHP_EOL;
ini_set('opcache.enable', 1);

?>
--EXPECTF--
Should warn, since the INI was initialized to 0:

Warning: Zend OPcache can't be temporarily enabled (it may be only disabled until the end of request) in %s on line %d
