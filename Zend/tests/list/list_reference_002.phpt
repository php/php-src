--TEST--
"Reference Unpacking - New Reference" list()
--FILE--
<?php
$arr = array(new stdclass);
list(&$a, &$b) = $arr;
var_dump($a, $b);
var_dump($arr);
?>
--EXPECT--
object(DynamicObject)#1 (0) {
}
NULL
array(2) {
  [0]=>
  &object(DynamicObject)#1 (0) {
  }
  [1]=>
  &NULL
}
