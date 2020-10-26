--TEST--
Test return nullsafe as ref
--FILE--
<?php

function &get_bar_ref($foo) {
    return $foo?->bar;
}

?>
--EXPECTF--
Fatal error: Cannot take reference of a null-safe chain in %s on line %d
