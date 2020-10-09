--TEST--
Test array_slice() function : usage variations - array has holes in buckets
--FILE--
<?php
/*
 * Check that results of array_slice are correct when there are holes in buckets caused by unset()
 */

echo "*** Testing array_slice() : usage variations ***\n";

function dump_slice(array $input, $offsetToUnset, int $offset, int $length) {
    unset($input[$offsetToUnset]);
    var_dump(array_slice($input, $offset, $length));
}

echo "\n-- Call array_slice() on array with string keys--\n";
$input = ['one' => 'un', 'two' => 'deux', 23 => 'twenty-three', 'zero'];
dump_slice($input, 'two', 0, 1);
dump_slice($input, 'two', 0, 2);
dump_slice($input, 'two', 0, 3);
dump_slice($input, 23, 1, 2);

echo "\n-- Call array_slice() on array with packed keys--\n";
$input = [10, 11, 12, 'thirteen'];
dump_slice($input, 0, 0, 1);
dump_slice($input, 1, 0, 1);
dump_slice($input, 1, 0, 3);
dump_slice($input, 1, -1, 1);
dump_slice($input, 1, 0, 3);
dump_slice($input, 1, -3, 3);

echo "Done";
?>
--EXPECT--
*** Testing array_slice() : usage variations ***

-- Call array_slice() on array with string keys--
array(1) {
  ["one"]=>
  string(2) "un"
}
array(2) {
  ["one"]=>
  string(2) "un"
  [0]=>
  string(12) "twenty-three"
}
array(3) {
  ["one"]=>
  string(2) "un"
  [0]=>
  string(12) "twenty-three"
  [1]=>
  string(4) "zero"
}
array(2) {
  ["two"]=>
  string(4) "deux"
  [0]=>
  string(4) "zero"
}

-- Call array_slice() on array with packed keys--
array(1) {
  [0]=>
  int(11)
}
array(1) {
  [0]=>
  int(10)
}
array(3) {
  [0]=>
  int(10)
  [1]=>
  int(12)
  [2]=>
  string(8) "thirteen"
}
array(1) {
  [0]=>
  string(8) "thirteen"
}
array(3) {
  [0]=>
  int(10)
  [1]=>
  int(12)
  [2]=>
  string(8) "thirteen"
}
array(3) {
  [0]=>
  int(10)
  [1]=>
  int(12)
  [2]=>
  string(8) "thirteen"
}
Done
