--TEST--
Float with fractional part offset behaviour
--FILE--
<?php

require dirname(__DIR__) . DIRECTORY_SEPARATOR . 'container_var.inc';
require dirname(__DIR__, 2) . DIRECTORY_SEPARATOR . 'test_const_float_with_fractional_offsets.inc';

?>
--EXPECTF--
Read before write:

Warning: String offset cast occurred in %s on line %d

Warning: Uninitialized string offset %d in %s on line %d
string(0) ""
Write:

Warning: String offset cast occurred in %s on line %d
Read:

Warning: String offset cast occurred in %s on line %d
string(1) "v"
Read-Write:

Warning: String offset cast occurred in %s on line %d
Cannot use assign-op operators with string offsets
isset():

Deprecated: Implicit conversion from float 5.5 to int loses precision in %s on line %d
bool(true)
empty():

Deprecated: Implicit conversion from float 5.5 to int loses precision in %s on line %d
bool(false)
Coalesce():
string(1) "v"
