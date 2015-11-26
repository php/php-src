--TEST--
Bug #45706 Unserialization of classes derived from ArrayIterator fails
--FILE--
<?php
class Foo1 extends ArrayIterator
{
}
class Foo2 {
}
$x = array(new Foo1(),new Foo2);
$s = serialize($x);
$s = str_replace("Foo", "Bar", $s);
$y = unserialize($s);
var_dump($y);
--EXPECTF--
Warning: Class __PHP_Incomplete_Class has no unserializer in %sbug45706.php on line %d
array(2) {
  [0]=>
  object(__PHP_Incomplete_Class)#%d (1) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(4) "Bar1"
  }
  [1]=>
  object(__PHP_Incomplete_Class)#%d (1) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(4) "Bar2"
  }
}
