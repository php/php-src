--TEST--
Serialization of backtick literal is incorrect
--INI--
zend.assertions=1
--FILE--
<?php

try {
    assert(false && `echo -n ""`);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
AssertionError: assert(false && `echo -n ""`)
