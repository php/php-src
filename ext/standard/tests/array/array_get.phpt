--TEST--
Test array_get() function
--FILE--
<?php
/*
 * Test functionality of array_get()
 */

echo "*** Testing array_get() ***\n";

// Basic array access
$array = ['products' => ['desk' => ['price' => 100]]];

// Test nested access with dot notation
var_dump(array_get($array, 'products.desk.price'));

// Test with default value when key doesn't exist
var_dump(array_get($array, 'products.desk.discount', 0));

// Test simple key access
$simple = ['name' => 'John', 'age' => 30];
var_dump(array_get($simple, 'name'));
var_dump(array_get($simple, 'missing', 'default'));

// Test with integer key
$indexed = ['a', 'b', 'c'];
var_dump(array_get($indexed, 0));
var_dump(array_get($indexed, 5, 'not found'));

// Test with null key (returns whole array)
$test = ['foo' => 'bar'];
var_dump(array_get($test, null));

// Test nested with missing intermediate key
var_dump(array_get($array, 'products.chair.price', 50));

// Test single level key that doesn't exist
var_dump(array_get($array, 'missing'));

// Test with numeric string in path (like users.0.name)
$users = ['users' => [['name' => 'Alice'], ['name' => 'Bob']]];
var_dump(array_get($users, 'users.0.name'));
var_dump(array_get($users, 'users.1.age', 70));

// Test with array key (equivalent to dot notation)
var_dump(array_get($array, ['products', 'desk', 'price']));
var_dump(array_get($simple, ['name']));
var_dump(array_get($users, ['users', 0, 'name']));
var_dump(array_get($array, ['products', 'chair', 'price'], 75));

// Test with invalid segment type in array key
var_dump(array_get($array, ['products', new stdClass(), 'price'], 'invalid'));

// Test with references - ensure returned value is a copy, not a reference
$ref_array = ['data' => ['value' => 'original']];
$ref =& $ref_array['data']['value'];
$result = array_get($ref_array, 'data.value');
var_dump($result);
$ref = 'modified';
var_dump($result); // Should still be 'original' (not affected by reference change)

// Test with default value being a reference
$default_value = 'default';
$default_ref =& $default_value;
$result_with_ref_default = array_get($ref_array, 'missing.key', $default_ref);
var_dump($result_with_ref_default);
$default_value = 'changed';
var_dump($result_with_ref_default); // Should still be 'default' (not affected by reference change)

echo "Done";
?>
--EXPECT--
*** Testing array_get() ***
int(100)
int(0)
string(4) "John"
string(7) "default"
string(1) "a"
string(9) "not found"
array(1) {
  ["foo"]=>
  string(3) "bar"
}
int(50)
NULL
string(5) "Alice"
int(70)
int(100)
string(4) "John"
string(5) "Alice"
int(75)
string(7) "invalid"
string(8) "original"
string(8) "original"
string(7) "default"
string(7) "default"
Done
