--TEST--
GH-15187: Crash in hooked iterators on properties array without dynamic properties
--FILE--
<?php

class Test {
    public int $prop { set => $value; }
}

$test = new Test();
var_dump($test);
foreach ($test as $key => $value) {
    var_dump($key, $value);
}

?>
--EXPECTF--
object(Test)#%d (0) {
  ["prop"]=>
  uninitialized(int)
}
