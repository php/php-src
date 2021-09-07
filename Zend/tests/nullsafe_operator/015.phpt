--TEST--
Test nullsafe in binary op
--FILE--
<?php

(new stdClass)->{null?->x}->y;

?>
--EXPECTF--
Warning: Undefined property: DynamicObject::$ in %s on line %d

Warning: Attempt to read property "y" on null in %s on line 3
