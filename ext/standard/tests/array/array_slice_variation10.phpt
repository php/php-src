--TEST--
Test array_slice() function : usage variations - position of internal array pointer
--FILE--
<?php
/* Prototype  : array array_slice(array $input, int $offset [, int $length [, bool $preserve_keys]])
 * Description: Returns elements specified by offset and length 
 * Source code: ext/standard/array.c
 */

/*
 * Check position of internal array pointer after calling array_slice()
 */

echo "*** Testing array_slice() : usage variations ***\n";

$input = array ('one' => 'un', 'two' => 'deux', 23 => 'twenty-three', 'zero');

echo "\n-- Call array_slice() --\n";
var_dump($result = array_slice($input, 2));

echo "-- Position of Internal Pointer in Result: --\n";
echo key($result) . " => " . current($result) . "\n";
echo "\n-- Position of Internal Pointer in Original Array: --\n";
echo key($input) . " => " . current ($input) . "\n";

echo "Done";
?>

--EXPECTF--
*** Testing array_slice() : usage variations ***

-- Call array_slice() --
array(2) {
  [0]=>
  string(12) "twenty-three"
  [1]=>
  string(4) "zero"
}
-- Position of Internal Pointer in Result: --
0 => twenty-three

-- Position of Internal Pointer in Original Array: --
one => un
Done