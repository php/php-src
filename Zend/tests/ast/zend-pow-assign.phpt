--TEST--
ZEND_POW_ASSIGN
--INI--
zend.assertions=1
--FILE--
<?php

try {
    assert(false && ($a **= 2));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
AssertionError: assert(false && ($a **= 2))
