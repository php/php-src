--TEST--
Test array_unique() function : usage variations - SORT_STRICT flag preserves type-distinct values
--FILE--
<?php
/*
* Testing array_unique() with SORT_STRICT flag which preserves
* type-distinct values that would otherwise be coerced to equal.
*/

echo "*** Testing array_unique() : SORT_STRICT flag ***\n";

echo "\n-- Type-distinct values preserved with SORT_STRICT --\n";
$values = [0, "0", null, false, ""];
$result = array_unique($values, SORT_STRICT);
var_dump($result);

echo "\n-- Same-type duplicates are still removed --\n";
$values = [1, 2, 1, 3, 2];
$result = array_unique($values, SORT_STRICT);
var_dump($result);

echo "\n-- String duplicates removed --\n";
$values = ["a", "b", "a", "c"];
$result = array_unique($values, SORT_STRICT);
var_dump($result);

echo "\n-- Integer 1 and string \"1\" are different with SORT_STRICT --\n";
$values = [1, "1", 1, "1"];
$result = array_unique($values, SORT_STRICT);
var_dump($result);

echo "Done";
?>
--EXPECT--
*** Testing array_unique() : SORT_STRICT flag ***

-- Type-distinct values preserved with SORT_STRICT --
array(5) {
  [0]=>
  int(0)
  [1]=>
  string(1) "0"
  [2]=>
  NULL
  [3]=>
  bool(false)
  [4]=>
  string(0) ""
}

-- Same-type duplicates are still removed --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [3]=>
  int(3)
}

-- String duplicates removed --
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [3]=>
  string(1) "c"
}

-- Integer 1 and string "1" are different with SORT_STRICT --
array(2) {
  [0]=>
  int(1)
  [1]=>
  string(1) "1"
}
Done
