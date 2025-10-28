--TEST--
Deprecated: malformed string cast to numeric types
--FILE--
<?php

// Test malformed string to int (trailing data)
var_dump((int) "123abc");

// Test non-numeric string to int
var_dump((int) "abc");

// Test malformed string to float (trailing data)
var_dump((float) "12.5xyz");

// Test non-numeric string to float
var_dump((float) "hello");

// Valid numeric strings should not emit deprecation
var_dump((int) "123");
var_dump((float) "12.5");

?>
--EXPECTF--
Deprecated: Implicit conversion from malformed string "123abc" to int is deprecated in %s on line %d
int(123)

Deprecated: Implicit conversion from malformed string "abc" to int is deprecated in %s on line %d
int(0)

Deprecated: Implicit conversion from malformed string "12.5xyz" to float is deprecated in %s on line %d
float(12.5)

Deprecated: Implicit conversion from malformed string "hello" to float is deprecated in %s on line %d
float(0)
int(123)
float(12.5)
