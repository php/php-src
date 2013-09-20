--TEST--
Indexing -- Convert string to array
--FILE--
<?php
require 'indexing_testing.inc';
$value = array(1);

run_test('', $value, true);
run_test('  bar', $value, true);
run_test('', $value, false);

?>
--EXPECTF--
Warning: Illegal string offset 'foo' in %s on line %d

Notice: Array to string conversion in %s on line %d
string(1) "A"

Warning: Illegal string offset 'foo' in %s on line %d

Notice: Array to string conversion in %s on line %d
string(5) "A bar"

Fatal error: [] operator not supported for strings in %s on line %d
