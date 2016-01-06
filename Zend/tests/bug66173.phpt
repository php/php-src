--TEST--
Bug #66173 (Object/Array cast loses numeric property/element accessibility)
--FILE--
<?php

$obj = new StdClass;
$obj->{0} = 1;
$obj->{'1'} = 2;
$obj->{"2"} = 4;

$arr = (array)$obj;

var_dump($arr);
var_dump($arr[0]);
var_dump($arr[1]);
var_dump($arr[2]);

?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(4)
}
int(1)
int(2)
int(4)
