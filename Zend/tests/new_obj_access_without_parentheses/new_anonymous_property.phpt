--TEST--
Immediate property access on new anonymous class object
--FILE--
<?php

echo new class {
    public $property = 'property';
}->property;

?>
--EXPECT--
property
