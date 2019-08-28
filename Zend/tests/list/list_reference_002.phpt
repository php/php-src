--TEST--
"Reference Unpacking - New Reference" list()
--FILE--
<?php
$arr = array(new StdClass);
list(&$a, &$b) = $arr;
var_dump($a, $b);
var_dump($arr);
?>
--EXPECTF--
object(StdClass)#%d (0) {
}
NULL
array(2) {
  [0]=>
  &object(StdClass)#%d (0) {
  }
  [1]=>
  &NULL
}
