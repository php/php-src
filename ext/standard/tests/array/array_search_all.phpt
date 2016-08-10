--TEST--
array_search() tests
--FILE--
<?php

$a = array(1=>0, 2=>1, 4=>3, 5=>3, "a"=>"b", "c"=>"d", "e"=>"3");

var_dump(array_search(9, $a, true, true));
var_dump(array_search(3, $a, true, true));
var_dump(array_search(3, $a, false, true));
var_dump(array_search("d", $a, true, true));

?>
Done
--EXPECTF--
array(0) {
}
array(2) {
  [0]=>
  int(4)
  [1]=>
  int(5)
}
array(3) {
  [0]=>
  int(4)
  [1]=>
  int(5)
  [2]=>
  string(1) "e"
}
array(1) {
  [0]=>
  string(1) "c"
}
Done
