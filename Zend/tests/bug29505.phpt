--TEST--
Bug #29505 (get_class_vars() severely broken when used with arrays)
--FILE--
<?php

class Test {
    public $empty = array();
    public $three = array(1, "b"=>"c", 3=>array());
}

var_dump(get_class_vars('Test'));

?>
--EXPECT--
array(2) {
  ["empty"]=>
  array(0) {
  }
  ["three"]=>
  array(3) {
    [0]=>
    int(1)
    ["b"]=>
    string(1) "c"
    [3]=>
    array(0) {
    }
  }
}
