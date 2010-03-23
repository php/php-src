--TEST--
SPL: Cloning an instance of ArrayObject which wraps an array. 
--FILE--
<?php
$a = array(1,2);
$aa1 =  new ArrayObject($a);
$a['p1'] = 'new element added to a before clone';

$aa2 = clone $aa1;

$a['p2'] = 'new element added to a after clone';
$aa1['new.aa1'] = 'new element added to aa1';
$aa2['new.aa2'] = 'new element added to aa2';
var_dump($a, $aa1, $aa2);
?>
--EXPECTF--
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  ["p1"]=>
  string(35) "new element added to a before clone"
  ["p2"]=>
  string(34) "new element added to a after clone"
}
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    ["new.aa1"]=>
    string(24) "new element added to aa1"
  }
}
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    ["new.aa2"]=>
    string(24) "new element added to aa2"
  }
}
