--TEST--
Float with fractional part offset behaviour
--FILE--
<?php

require dirname(__DIR__) . DIRECTORY_SEPARATOR . 'container_var.inc';
require dirname(__DIR__, 2) . DIRECTORY_SEPARATOR . 'dimension_var_float_with_fractional.inc';
require dirname(__DIR__, 2) . DIRECTORY_SEPARATOR . 'test_variable_offsets.inc';

?>
--EXPECTF--
Read before write:

Warning: Trying to access array offset on null in %s on line %d
NULL
Write:

Deprecated: Implicit conversion from float 5.5 to int loses precision in %s on line %d
Read:

Deprecated: Implicit conversion from float 5.5 to int loses precision in %s on line %d
string(1) "v"
Read-Write:

Deprecated: Implicit conversion from float 5.5 to int loses precision in %s on line %d
isset():

Deprecated: Implicit conversion from float 5.5 to int loses precision in %s on line %d
bool(true)
empty():

Deprecated: Implicit conversion from float 5.5 to int loses precision in %s on line %d
bool(false)
Coalesce():

Deprecated: Implicit conversion from float 5.5 to int loses precision in %s on line %d
string(7) "vappend"
