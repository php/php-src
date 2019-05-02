--TEST--
Pretty printing for arrow functions
--FILE--
<?php

// TODO Use arrow function in output.
assert((fn() => false)());

?>
--EXPECT--
Warning: assert(): assert(function () use() {
    return false;
}()) failed in /home/nikic/php-7.4/Zend/tests/arrow_functions/007.php on line 3
