--TEST--
Test array_has() function
--FILE--
<?php
/*
 * Test functionality of array_has()
 */

echo "*** Testing array_has() ***\n";

// Basic array
$array = ['product' => ['name' => 'Desk', 'price' => 100]];

// Test nested key exists with dot notation
var_dump(array_has($array, 'product.name'));

// Test nested key doesn't exist
var_dump(array_has($array, 'product.color'));

// Test intermediate key doesn't exist
var_dump(array_has($array, 'category.name'));

// Test simple key access
$simple = ['name' => 'John', 'age' => 30];
var_dump(array_has($simple, 'name'));
var_dump(array_has($simple, 'missing'));

// Test with integer key
$indexed = ['a', 'b', 'c'];
var_dump(array_has($indexed, 0));
var_dump(array_has($indexed, 1));
var_dump(array_has($indexed, 5));

// Test with value that is null (key exists, but value is null)
$withNull = ['key' => null];
var_dump(array_has($withNull, 'key'));

// Test with numeric string in path (like users.0.name)
$users = ['users' => [['name' => 'Alice'], ['name' => 'Bob']]];
var_dump(array_has($users, 'users.0.name'));
var_dump(array_has($users, 'users.1.age'));
var_dump(array_has($users, 'users.2.name'));

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
bool(true)
bool(false)
bool(false)
Done
