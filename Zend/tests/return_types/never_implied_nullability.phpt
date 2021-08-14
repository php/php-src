--TEST--
never type: cannot use never with implied nullability
--FILE--
<?php

function foo(never $var = null) {
    echo $var;
}

foo("bar");

?>
--EXPECTF--
Fatal error: never can only be used as a standalone type in %s on line %d