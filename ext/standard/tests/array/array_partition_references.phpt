--TEST--
Test array_partition() function : by-reference array elements
--FILE--
<?php

echo "*** Reference elements behave like array_filter ***\n";
$input = [1, 2, 3, 4];
$ref = &$input[0];
$ref2 = &$input[1];

$filtered = array_filter($input, fn($v) => $v % 2);
[$matching, $nonMatching] = array_partition($input, fn($v) => $v % 2);

$ref = 100;
$ref2 = 200;

var_dump($matching === $filtered);
var_dump($matching, $nonMatching);

echo "*** Input array is not modified ***\n";
var_dump($input);

echo "*** By-reference callback parameter cannot modify the input ***\n";
$input = ['a' => 1, 'b' => 2];
[$matching, $nonMatching] = array_partition($input, function (&$v) {
    $v = 'changed';
    return true;
});
var_dump($input, $matching);

?>
--EXPECTF--
*** Reference elements behave like array_filter ***
bool(true)
array(2) {
  [0]=>
  &int(100)
  [2]=>
  int(3)
}
array(2) {
  [1]=>
  &int(200)
  [3]=>
  int(4)
}
*** Input array is not modified ***
array(4) {
  [0]=>
  &int(100)
  [1]=>
  &int(200)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
*** By-reference callback parameter cannot modify the input ***

Warning: {closure:%s:%d}(): Argument #1 ($v) must be passed by reference, value given in %s on line %d

Warning: {closure:%s:%d}(): Argument #1 ($v) must be passed by reference, value given in %s on line %d
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
