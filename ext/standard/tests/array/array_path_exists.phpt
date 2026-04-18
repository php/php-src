--TEST--
Test array_path_exists() function
--FILE--
<?php
echo "*** Testing array_path_exists() ***\n";

// Basic array
$array = ['product' => ['name' => 'Desk', 'price' => 100]];

// Test nested key exists with array path
var_dump(array_path_exists($array, ['product', 'name']));

// Test nested key doesn't exist
var_dump(array_path_exists($array, ['product', 'color']));

// Test intermediate key doesn't exist
var_dump(array_path_exists($array, ['category', 'name']));

// Test simple path with single level
$simple = ['name' => 'John', 'age' => 30];
var_dump(array_path_exists($simple, ['name']));
var_dump(array_path_exists($simple, ['missing']));

// Test with integer key in path
$users = ['users' => [['name' => 'Alice'], ['name' => 'Bob']]];
var_dump(array_path_exists($users, ['users', 0, 'name']));
var_dump(array_path_exists($users, ['users', 1, 'name']));
var_dump(array_path_exists($users, ['users', 2, 'name']));

// Test with value that is null (key exists, but value is null)
$withNull = ['key' => null];
var_dump(array_path_exists($withNull, ['key']));

// Test with invalid segment type in array path
try {
	var_dump(array_path_exists($array, ['product', new stdClass()]));
} catch (TypeError $e) {
	echo $e->getMessage() . "\n";
}

// Test with reference to an array in the path
$array2 = ['world'];
$array_with_ref = ['hello' => &$array2];
var_dump(array_path_exists($array_with_ref, ['hello', 0]));

// Test with path segment that is a reference
$key1 = 'product';
$key2 = 'name';
$path_with_refs = [&$key1, &$key2];
var_dump(array_path_exists($array, $path_with_refs));

echo "Done";
?>
--EXPECT--
*** Testing array_path_exists() ***
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
Path segment must be of type string|int, stdClass given
bool(true)
bool(true)
Done
