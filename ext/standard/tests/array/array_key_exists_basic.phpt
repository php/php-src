--TEST--
Test array_key_exists() function : basic functionality
--FILE--
<?php
/* Prototype  : bool array_key_exists(mixed $key, array $search)
 * Description: Checks if the given key or index exists in the array
 * Source code: ext/standard/array.c
 * Alias to functions: key_exists
 */

/*
 * Test basic functionality of array_key_exists()
 */

echo "*** Testing array_key_exists() : basic functionality ***\n";

$key1 = 'key';
$key2 = 'val';
$search = array('one', 'key' => 'value', 'val');
var_dump(array_key_exists($key1, $search));
var_dump(array_key_exists($key2, $search));

echo "Done";
?>
--EXPECTF--
*** Testing array_key_exists() : basic functionality ***
bool(true)
bool(false)
Done
