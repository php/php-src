--TEST--
Bug #27395 (reference to an array index makes the array to be passed by reference always)
--FILE--
<?php

function theFunction($arg) {
	$arg[0] = 2;
}

// Reference on array index
$arr1 = array (1);
$reference1 =& $arr1[0];
    
var_dump($reference1);
var_dump($arr1);
theFunction($arr1);
var_dump($reference1);
var_dump($arr1);

echo "--------\n";

// Reference on array
$arr2 = array (1);
$reference2 =& $arr2;
    
var_dump($reference2);
var_dump($arr2);
theFunction($arr2);
var_dump($reference2);
var_dump($arr2);

?>
--EXPECT--
int(1)
array(1) {
  [0]=>
  &int(1)
}
int(1)
array(1) {
  [0]=>
  &int(1)
}
--------
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(1)
}
