--TEST--
Deprecated: null cast to scalar types
--FILE--
<?php

// Test null to int
var_dump((int) null);

// Test null to float
var_dump((float) null);

// Test null to string
var_dump((string) null);

// Test null to bool
var_dump((bool) null);

// Test null to array
var_dump((array) null);

// Test null to object
var_dump((object) null);

?>
--EXPECTF--
Deprecated: Implicit conversion from null to int is deprecated in %s on line %d
int(0)

Deprecated: Implicit conversion from null to float is deprecated in %s on line %d
float(0)

Deprecated: Implicit conversion from null to string is deprecated in %s on line %d
string(0) ""

Deprecated: Implicit conversion from null to bool is deprecated in %s on line %d
bool(false)

Deprecated: Implicit conversion from null to array is deprecated in %s on line %d
array(0) {
}

Deprecated: Implicit conversion from null to object is deprecated in %s on line %d
object(stdClass)#1 (0) {
}
