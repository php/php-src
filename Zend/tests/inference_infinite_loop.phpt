--TEST--
Type inference should not result in infinite loop
--FILE--
<?php

function test() {
    $b = false;
    do {
        $a = $a + PHP_INT_MAX + 2;
        $a = 0;
    } while ($b);
}
test();

?>
--EXPECTF--
Warning: Undefined variable $a (this will become an error in PHP 9.0) in %s on line %d
