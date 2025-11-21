--TEST--
Deprecated: object cast to int/float/bool
--FILE--
<?php

class SimpleObject {}
$obj = new SimpleObject();

// Test object to int
var_dump((int) $obj);

// Test object to float
var_dump((float) $obj);

// Test object to bool
var_dump((bool) $obj);

?>
--EXPECTF--
Deprecated: Implicit conversion from SimpleObject to int is deprecated in %s on line %d

Warning: Object of class SimpleObject could not be converted to int in %s on line %d
int(1)

Deprecated: Implicit conversion from SimpleObject to float is deprecated in %s on line %d

Warning: Object of class SimpleObject could not be converted to float in %s on line %d
float(1)

Deprecated: Implicit conversion from SimpleObject to bool is deprecated in %s on line %d
bool(true)
