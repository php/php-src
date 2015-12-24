--TEST--
Bug #71208 (Array/Object cast loses numeric property/element accessibility)
--FILE--
<?php

$arr = [1, 2, 4];
$obj = (object)$arr;

var_dump($obj);
var_dump($obj->{0}, $obj->{1}, $obj->{2});
var_dump($obj->{'0'}, $obj->{'1'}, $obj->{'2'});

?>
--EXPECTF--
object(stdClass)#1 (3) {
  ["0"]=>
  int(1)
  ["1"]=>
  int(2)
  ["2"]=>
  int(4)
}
int(1)
int(2)
int(4)
int(1)
int(2)
int(4)
