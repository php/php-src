--TEST--
test enable/disable assertions at runtime (assertions not completely disabled)
--INI--
zend.assertions=0
--FILE--
<?php
ini_set("zend.assertions", 0);
var_dump(assert(false));
var_dump(assert(true));
ini_set("zend.assertions", 1);
try {
    var_dump(assert(false));
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}
var_dump(assert(true));
ini_set("zend.assertions", -1);
?>
--EXPECTF--
bool(true)
bool(true)
assert(): assert(false) failed
bool(true)

Warning: zend.assertions may be completely enabled or disabled only in php.ini in %s on line %d
