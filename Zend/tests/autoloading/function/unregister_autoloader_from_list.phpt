--TEST--
Unregister all autoloaders by traversing the registered list
--FILE--
<?php

function f1($class) { echo "f1: [[$class]]\n"; }
function f2($class) { echo "f2: [[$class]]\n"; }

autoload_register_function('f1');
autoload_register_function('f2');
autoload_register_function(function($class) { echo "cf1: [[$class]]\n"; });
autoload_register_function(function($class) { echo "cf2: [[$class]]\n"; });

foreach (autoload_list_function() as $func) {
    autoload_unregister_function($func);
}

var_dump(autoload_list_function());
?>
--EXPECT--
array(0) {
}
