--TEST--
Bug #42208 (substr_replace() crashes when the same array is passed more than once)
--FILE--
<?php
$a = array(1, 2);
$c = $a;
var_dump(substr_replace($a, 1, 1, $c));
?>
--EXPECT--
array(2) {
  [0]=>
  string(2) "11"
  [1]=>
  string(2) "21"
}
