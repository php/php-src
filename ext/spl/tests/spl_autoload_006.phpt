--TEST--
SPL: spl_autoload() with static methods
--INI--
include_path=.
--FILE--
<?php

class MyAutoLoader {

        static function autoLoad($className) {
        	echo __METHOD__ . "($className)\n";
        }
}

spl_autoload_register('MyAutoLoader::autoLoad');

var_dump(spl_autoload_functions());

// check
var_dump(class_exists("TestClass", true));

?>
===DONE===
<?php exit(0); ?>
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
===DONE===
