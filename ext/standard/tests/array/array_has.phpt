--TEST--
Test array_has() function
--FILE--
<?php
echo "*** Testing array_has() ***\n";

// Basic array
$array = ['product' => ['name' => 'Desk', 'price' => 100]];

// Test nested key exists with array path
var_dump(array_has($array, ['product', 'name']));

// Test nested key doesn't exist
var_dump(array_has($array, ['product', 'color']));

// Test intermediate key doesn't exist
var_dump(array_has($array, ['category', 'name']));

// Test simple path with single level
$simple = ['name' => 'John', 'age' => 30];
var_dump(array_has($simple, ['name']));
var_dump(array_has($simple, ['missing']));

// Test with integer key in path
$users = ['users' => [['name' => 'Alice'], ['name' => 'Bob']]];
var_dump(array_has($users, ['users', 0, 'name']));
var_dump(array_has($users, ['users', 1, 'name']));
var_dump(array_has($users, ['users', 2, 'name']));

// Test with value that is null (key exists, but value is null)
$withNull = ['key' => null];
var_dump(array_has($withNull, ['key']));

// Test with invalid segment type in array path
var_dump(array_has($array, ['product', new stdClass()]));

echo "Done";
?>
--EXPECT--
*** Testing array_has() ***
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
Done
