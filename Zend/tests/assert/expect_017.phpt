--TEST--
test enable/disable assertions at runtime (assertions completely disabled)
--INI--
zend.assertions=-1
--FILE--
<?php
ini_set("zend.assertions", 0);
var_dump(assert(false));
var_dump(assert(true));
ini_set("zend.assertions", 1);
?>
--EXPECTF--
Warning: zend.assertions may be completely enabled or disabled only in php.ini in %sexpect_017.php on line 2
bool(true)
bool(true)

Warning: zend.assertions may be completely enabled or disabled only in php.ini in %sexpect_017.php on line 5
