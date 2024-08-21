--TEST--
Autoloader is a static method
--FILE--
<?php

class MyAutoLoader {
    static function autoLoad($className) {
        echo __METHOD__ . "($className)\n";
    }
}

autoload_register_class('MyAutoLoader::autoLoad');

var_dump(autoload_list_class());

// check
var_dump(class_exists("TestClass", true));

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
MyAutoLoader::autoLoad(TestClass)
bool(false)
