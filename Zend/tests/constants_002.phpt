--TEST--
Defining constants with non-scalar values
--FILE--
<?php

define('foo', new stdClass);
var_dump(foo);

define('foo', fopen(__FILE__, 'r'));
var_dump(foo);

?>
--EXPECTF--
Warning: Constants may only evaluate to scalar values in %s on line %d

Notice: Use of undefined constant foo - assumed 'foo' in %s on line %d
string(%d) "foo"
resource(%d) of type (stream)
