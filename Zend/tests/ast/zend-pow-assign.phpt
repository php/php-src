--TEST--
ZEND_POW_ASSIGN
--INI--
zend.assertions=1
--FILE--
<?php

try {
    assert(false && ($a **= 2));
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}
?>
--EXPECT--
assert(): assert(false && ($a **= 2)) failed
