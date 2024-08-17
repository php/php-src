--TEST--
Bug #65006: autoload_register_class fails with multiple callables using self, same method
--FILE--
<?php

class first {
    public static function init() {
        autoload_register_class(array('self','load'));
    }
    public static function load($class) {}
}

class second {
    public static function init() {
        autoload_register_class(array('self','load'));
    }
    public static function load($class){}
}

first::init();
second::init();
var_dump(autoload_list_class());

?>
--EXPECTF--
Deprecated: Use of "self" in callables is deprecated in %s on line %d

Deprecated: Use of "self" in callables is deprecated in %s on line %d
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(4) "load"
  }
  [1]=>
  array(2) {
    [0]=>
    string(6) "second"
    [1]=>
    string(4) "load"
  }
}
