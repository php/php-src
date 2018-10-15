--TEST--
Test array_key_exists() function : usage variations - position of internal array pointer
--FILE--
<?php
/* Prototype  : bool array_key_exists(mixed $key, array $search)
 * Description: Checks if the given key or index exists in the array
 * Source code: ext/standard/array.c
 * Alias to functions: key_exists
 */

/*
 * Check the position of the internal array pointer after calling array_key_exists()
 */

echo "*** Testing array_key_exists() : usage variations ***\n";

$input = array ('one' => 'un', 'two' => 'deux', 'three' => 'trois');

echo "\n-- Call array_key_exists() --\n";
var_dump($result = array_key_exists('one', $input));

echo "\n-- Position of Internal Pointer in Original Array: --\n";
echo key($input) . " => " . current ($input) . "\n";

echo "Done";
?>
--EXPECTF--
*** Testing array_key_exists() : usage variations ***

-- Call array_key_exists() --
bool(true)

-- Position of Internal Pointer in Original Array: --
one => un
Done
