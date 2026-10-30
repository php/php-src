--TEST--
test assertions in namespace
--INI--
zend.assertions=1
--FILE--
<?php
namespace Foo;

ini_set("zend.assertions", 0);
var_dump(\assert(false));
var_dump(\assert(true));
var_dump(assert(false));
var_dump(assert(true));
ini_set("zend.assertions", 1);
try {
    var_dump(\assert(false));
} catch (\AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}
var_dump(\assert(true));
try {
    var_dump(assert(false));
} catch (\AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}
var_dump(assert(true));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
assert(): assert(false) failed
bool(true)
assert(): assert(false) failed
bool(true)
