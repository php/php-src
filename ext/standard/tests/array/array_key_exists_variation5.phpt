--TEST--
Test array_key_exists() function : usage variations - multidimensional arrays
--FILE--
<?php
/* Prototype  : bool array_key_exists(mixed $key, array $search)
 * Description: Checks if the given key or index exists in the array
 * Source code: ext/standard/array.c
 * Alias to functions: key_exists
 */

/*
 * Test how array_key_exists() behaves with multi-dimensional arrays
 */

echo "*** Testing array_key_exists() : usage variations ***\n";

$multi_array = array ('zero' => 'val1',
                      'one' => 'val2',
                      'sub1' => array (1, 2, 3));

echo "\n-- Attempt to match key in sub-array --\n";
// this key is in the sub-array
var_dump(array_key_exists(0, $multi_array));

echo "\n-- \$search arg points to sub-array --\n";
var_dump(array_key_exists(0, $multi_array['sub1']));

echo "Done";
?>
--EXPECT--
*** Testing array_key_exists() : usage variations ***

-- Attempt to match key in sub-array --
bool(false)

-- $search arg points to sub-array --
bool(true)
Done
