--TEST--
Test array_key_exists() function : usage variations - referenced variables
--INI--
allow_call_time_pass_reference=on
--FILE--
<?php
/* Prototype  : bool array_key_exists(mixed $key, array $search)
 * Description: Checks if the given key or index exists in the array 
 * Source code: ext/standard/array.c
 * Alias to functions: key_exists
 */

/*
 * Pass referenced variables as arguments to array_key_exists() to test behaviour
 */

echo "*** Testing array_key_exists() : usage variations ***\n";

$array = array('one' => 1, 'two' => 2, 'three' => 3);

echo "\n-- \$search is a reference to \$array --\n";
$search = &$array;
var_dump(array_key_exists('one', $search));

echo "\n-- \$key is a referenced variable --\n";
$key = 'two';
var_dump(array_key_exists(&$key, $array));

echo "\n-- Both arguments are referenced variables --\n";
var_dump(array_key_exists(&$key, &$array));

echo "Done";
?>

--EXPECTF--
*** Testing array_key_exists() : usage variations ***

-- $search is a reference to $array --
bool(true)

-- $key is a referenced variable --
bool(true)

-- Both arguments are referenced variables --
bool(true)
Done
