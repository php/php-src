--TEST--
Autoloader is a static method
--FILE--
<?php

class MyAutoLoader {
    static function autoLoad($className) {
        echo __METHOD__ . "($className)\n";
    }
}

autoload_register_function('MyAutoLoader::autoLoad');

var_dump(autoload_list_function());

// check
var_dump(function_exists("foo", true));

?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(12) "MyAutoLoader"
    [1]=>
    string(8) "autoLoad"
  }
}
MyAutoLoader::autoLoad(foo)
bool(false)
