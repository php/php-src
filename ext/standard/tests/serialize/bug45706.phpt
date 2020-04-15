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
array(2) {
  [0]=>
  object(__PHP_Incomplete_Class)#3 (5) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(4) "Bar1"
    ["0"]=>
    int(0)
    ["1"]=>
    array(0) {
    }
    ["2"]=>
    array(0) {
    }
    ["3"]=>
    NULL
  }
  [1]=>
  object(__PHP_Incomplete_Class)#4 (1) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(4) "Bar2"
  }
}
