--TEST--
List evaluation order should be reversed
--FILE--
<?php

$list = array('first', 'second', 'third');
list($a[0], $a[1], $a[2]) = $list;

var_dump($a);

?>
--EXPECTF--
array(3) {
  [2]=>
  string(5) "third"
  [1]=>
  string(6) "second"
  [0]=>
  string(5) "first"
}
