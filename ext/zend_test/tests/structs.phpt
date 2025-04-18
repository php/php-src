--TEST--
Internal structs
--EXTENSIONS--
zend_test
--FILE--
<?php

$box = new ZendTestBox();
$box->value = 1;
$copy = $box;
$copy->value++;
var_dump($box);
var_dump($copy);
$copy = $box;
$copy->setNull!();
var_dump($box);
var_dump($copy);

?>
--EXPECT--
object(ZendTestBox)#1 (1) {
  ["value"]=>
  int(1)
}
object(ZendTestBox)#2 (1) {
  ["value"]=>
  int(2)
}
object(ZendTestBox)#1 (1) {
  ["value"]=>
  int(1)
}
object(ZendTestBox)#2 (1) {
  ["value"]=>
  NULL
}
