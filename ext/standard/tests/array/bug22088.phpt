--TEST--
Bug #21998 (array_shift() leaves next index to be +1 too much)
--FILE--
<?php

$a = array('a', 'b', 'c');
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
