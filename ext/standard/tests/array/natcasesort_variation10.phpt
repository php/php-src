--TEST--
Test natcasesort() function : usage variations - position of internal array pointer
--FILE--
<?php
/* Prototype  : bool natcasesort(array &$array_arg)
 * Description: Sort an array using case-insensitive natural sort
 * Source code: ext/standard/array.c
 */

/*
 * Check position of internal array pointer after calling natcasesort()
 */

echo "*** Testing natcasesort() : usage variations ***\n";

$array_arg = array ('img13', 'img20', 'img2', 'img1');

echo "\n-- Initial Position of Internal Pointer: --\n";
echo key($array_arg) . " => " . current ($array_arg) . "\n";

echo "\n-- Call natcasesort() --\n";
var_dump(natcasesort($array_arg));
var_dump($array_arg);

echo "\n-- Position of Internal Pointer in Passed Array: --\n";
echo key($array_arg) . " => " . current ($array_arg) . "\n";

echo "Done";
?>
--EXPECT--
*** Testing natcasesort() : usage variations ***

-- Initial Position of Internal Pointer: --
0 => img13

-- Call natcasesort() --
bool(true)
array(4) {
  [3]=>
  string(4) "img1"
  [2]=>
  string(4) "img2"
  [0]=>
  string(5) "img13"
  [1]=>
  string(5) "img20"
}

-- Position of Internal Pointer in Passed Array: --
3 => img1
Done
