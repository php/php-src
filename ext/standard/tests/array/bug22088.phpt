--TEST--
Bug #22088 (array_shift() leaves next index to be +1 too much)
--FILE--
<?php

$a = array('a', 'b', 'c');
$last = array_shift ($a);
$a[] = 'a';
var_dump($a);

$a = array('a' => 1, 'b' => 2, 'c' => 3);
$last = array_shift ($a);
$a[] = 'a';
var_dump($a);

?>
--EXPECT--
array(3) {
  [0]=>
  string(1) "b"
  [1]=>
  string(1) "c"
  [2]=>
  string(1) "a"
}
array(3) {
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
  [0]=>
  string(1) "a"
}
