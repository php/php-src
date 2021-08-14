--TEST--
never type: cannot use never with unions
--FILE--
<?php

function foo(never|string $var) {
    echo $var;
}

foo("bar");

?>
--EXPECTF--
Fatal error: never can only be used as a standalone type in %s on line %d