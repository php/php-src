--TEST--
Serialization of backtick literal is incorrect
--INI--
zend.assertions=1
--FILE--
<?php

try {
    assert(false && `echo -n ""`);
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}

?>
--EXPECT--
assert(): assert(false && `echo -n ""`) failed
