--TEST--
Serialization of backtick literal is incorrect
--INI--
zend.assertions=1
--FILE--
<?php

try {
    assert(false && `echo -n ""`);
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
AssertionError: assert(false && `echo -n ""`)
