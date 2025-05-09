--TEST--
GH-7900: Arrow function with never return type compile-time errors (multiline, Fatal error)
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

try {
    assert((fn(): never => { return 42; }) && false);
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Fatal error: A never-returning function must not return in %s on line %d
