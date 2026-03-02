--TEST--
Pretty printing for arrow functions
--INI--
zend.assertions=1
--FILE--
<?php

try {
    assert((function() { return false; })());
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}

?>
--EXPECT--
assert(): assert((function () {
    return false;
})()) failed
