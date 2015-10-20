--TEST--
Default values for boolean hints should work
--FILE--
<?php

function foo(bool $x = true, bool $y = false) {
    var_dump($x, $y);
}



foo();
?>
--EXPECTF--
bool(true)
bool(false)
