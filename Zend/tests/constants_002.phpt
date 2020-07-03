--TEST--
Defining constants with non-scalar values should throw an error
--FILE--
<?php

require __DIR__ . '/constants_helpers.inc';

tchelper_define('foo', new stdClass);
tchelper_define('foo', fopen(__FILE__, 'r'));

?>
--EXPECT--
>> define("foo", object);
TypeError :: Constants may only evaluate to scalar values, arrays or resources

>> define("foo", resource);
true
