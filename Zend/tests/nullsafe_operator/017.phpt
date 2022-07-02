--TEST--
Test return nullsafe as ref
--FILE--
<?php

function &get_bar_ref($foo) {
    return $foo?->bar;
}

?>
--EXPECTF--
Fatal error: Cannot take reference of a nullsafe chain in %s.php on line 4
