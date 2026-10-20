--TEST--
Dynamically setting opcache.enable does not warn when noop
--INI--
opcache.enable=1
opcache.enable_cli=1
--EXTENSIONS--
opcache
--FILE--
<?php

echo "Should not warn:", PHP_EOL;
ini_set('opcache.enable', 1);
echo "Disabling:", PHP_EOL;
ini_set('opcache.enable', 0);
echo "Should warn:", PHP_EOL;
ini_set('opcache.enable', 1);

?>
--EXPECTF--
Should not warn:
Disabling:
Should warn:

Warning: Zend OPcache can't be temporarily enabled (it may be only disabled until the end of request) in %s on line %d
