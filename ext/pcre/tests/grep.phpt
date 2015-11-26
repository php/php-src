--TEST--
preg_grep()
--FILE--
<?php
$array = array('a', '1', 'q6', 'h20');

var_dump(preg_grep('/^(\d|.\d)$/', $array));
var_dump(preg_grep('/^(\d|.\d)$/', $array, PREG_GREP_INVERT));

?>
--EXPECT--
array(2) {
  [1]=>
  string(1) "1"
  [2]=>
  string(2) "q6"
}
array(2) {
  [0]=>
  string(1) "a"
  [3]=>
  string(3) "h20"
}
