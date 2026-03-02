--TEST--
Bug #77298 (segfault occurs when add property to unserialized ArrayObject)
--FILE--
<?php
$o = new ArrayObject();
$o2 = unserialize(serialize($o));
$o2[1]=123;
var_dump($o2);

$o3 = new ArrayObject();
$o3->unserialize($o->serialize());
$o3['xm']=456;
var_dump($o3);
?>
--EXPECT--
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    [1]=>
    int(123)
  }
}
object(ArrayObject)#3 (1) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    ["xm"]=>
    int(456)
  }
}
