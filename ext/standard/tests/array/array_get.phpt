--TEST--
Test array_get() function
--FILE--
<?php
echo "*** Testing array_get() ***\n";

// Basic nested array access
$array = ['products' => ['desk' => ['price' => 100]]];

// Test nested access with array path
var_dump(array_get($array, ['products', 'desk', 'price']));

// Test with default value when path doesn't exist
var_dump(array_get($array, ['products', 'desk', 'discount'], 5));

// Test simple path with single level
$simple = ['name' => 'John', 'age' => 30];
var_dump(array_get($simple, ['name']));
var_dump(array_get($simple, ['missing'], 'default'));

// Test single level key that doesn't exist
var_dump(array_get($array, ['missing']));

// Test with integer key in path
$users = ['users' => [['name' => 'Alice'], ['name' => 'Bob']]];
var_dump(array_get($users, ['users', 0, 'name']));
var_dump(array_get($users, ['users', 1, 'name']));

// Test nested with missing intermediate key
var_dump(array_get($array, ['products', 'chair', 'price'], 75));

// Test with invalid segment type in array path
var_dump(array_get($array, ['products', new stdClass(), 'price'], 'invalid'));

// Test with references - ensure returned value is a copy, not a reference
$ref_array = ['data' => ['value' => 'original']];
$ref =& $ref_array['data']['value'];
$result = array_get($ref_array, ['data', 'value']);
var_dump($result);
$ref = 'modified';
var_dump($result); // Should still be 'original' (not affected by reference change)

// Test with default value being a reference
$default_value = 'default';
$default_ref =& $default_value;
$result_with_ref_default = array_get($ref_array, ['missing', 'key'], $default_ref);
var_dump($result_with_ref_default);
$default_value = 'changed';
var_dump($result_with_ref_default); // Should still be 'default' (not affected by reference change)

echo "Done";
?>
--EXPECT--
*** Testing array_get() ***
int(100)
int(5)
string(4) "John"
string(7) "default"
NULL
string(5) "Alice"
string(3) "Bob"
int(75)
string(7) "invalid"
string(8) "original"
string(8) "original"
string(7) "default"
string(7) "default"
Done
