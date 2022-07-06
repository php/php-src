--TEST--
Trying to create a constant on Trait
--FILE--
<?php

trait foo {
    const a = 1;
}

?>
--EXPECTF--
Fatal error: Traits cannot have constants in %s on line %d
