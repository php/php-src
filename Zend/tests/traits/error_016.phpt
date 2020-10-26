--TEST--
Trying to create a constant on Trait
--FILE--
<?php

trait foo {
    const a = 1;
}

?>
--EXPECTF--
Fatal error: Trait foo cannot define constants in %s on line %d
