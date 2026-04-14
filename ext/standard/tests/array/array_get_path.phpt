--TEST--
Test array_get_path() function
--FILE--
<?php
echo "*** Testing array_get_path() ***\n";

// Basic nested array access
$array = ['products' => ['desk' => ['price' => 100]]];

// Test nested access with array path
var_dump(array_get_path($array, ['products', 'desk', 'price']));

// Test with default value when path doesn't exist
var_dump(array_get_path($array, ['products', 'desk', 'discount'], 5));

// Test simple path with single level
$simple = ['name' => 'John', 'age' => 30];
var_dump(array_get_path($simple, ['name']));
var_dump(array_get_path($simple, ['missing'], 'default'));

// Test single level key that doesn't exist
var_dump(array_get_path($array, ['missing']));

// Test with integer key in path
$users = ['users' => [['name' => 'Alice'], ['name' => 'Bob']]];
var_dump(array_get_path($users, ['users', 0, 'name']));
var_dump(array_get_path($users, ['users', 1, 'name']));

// Test nested with missing intermediate key
var_dump(array_get_path($array, ['products', 'chair', 'price'], 75));

// Test with invalid segment type in array path
try {
	var_dump(array_get_path($array, ['products', new stdClass(), 'price'], 'invalid'));
} catch (TypeError $e) {
	echo $e->getMessage() . "\n";
}

// Test with references - ensure returned value is a copy, not a reference
$ref_array = ['data' => ['value' => 'original']];
$ref =& $ref_array['data']['value'];
$result = array_get_path($ref_array, ['data', 'value']);
var_dump($result);
$ref = 'modified';
var_dump($result); // Should still be 'original' (not affected by reference change)

// Test with default value being a reference
$default_value = 'default';
$default_ref =& $default_value;
$result_with_ref_default = array_get_path($ref_array, ['missing', 'key'], $default_ref);
var_dump($result_with_ref_default);
$default_value = 'changed';
var_dump($result_with_ref_default); // Should still be 'default' (not affected by reference change)

// Test with reference to an array in the path
$array2 = ['world'];
$array_with_ref = ['hello' => &$array2];
var_dump(array_get_path($array_with_ref, ['hello', 0]));

// Test with path segment that is a reference
$key1 = 'products';
$key2 = 'desk';
$path_with_refs = [&$key1, &$key2, 'price'];
var_dump(array_get_path($array, $path_with_refs));

echo "Done";
?>
--EXPECT--
*** Testing array_get_path() ***
int(100)
int(5)
string(4) "John"
string(7) "default"
NULL
string(5) "Alice"
string(3) "Bob"
int(75)
Path segment must be of type string|int, stdClass given
string(8) "original"
string(8) "original"
string(7) "default"
string(7) "default"
string(5) "world"
int(100)
Done
