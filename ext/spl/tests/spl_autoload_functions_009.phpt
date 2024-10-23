--TEST--
can retrieve autoloader for functions and classes
--FILE--
<?php

spl_autoload_register($classes = function($class) {
    echo "class $class\n";
}, type: SPL_AUTOLOAD_CLASS);

spl_autoload_register($functions = function($function) {
    echo "function $function\n";
}, type: SPL_AUTOLOAD_FUNCTION);

spl_autoload_call('AClass', SPL_AUTOLOAD_CLASS);
spl_autoload_call('AFunction', SPL_AUTOLOAD_FUNCTION);

var_dump(in_array($classes, spl_autoload_functions(SPL_AUTOLOAD_CLASS), true));
var_dump(!in_array($classes, spl_autoload_functions(SPL_AUTOLOAD_FUNCTION), true));
var_dump(in_array($functions, spl_autoload_functions(SPL_AUTOLOAD_FUNCTION), true));
var_dump(!in_array($functions, spl_autoload_functions(SPL_AUTOLOAD_CLASS), true));
var_dump(in_array($classes, spl_autoload_functions(SPL_AUTOLOAD_CLASS | SPL_AUTOLOAD_FUNCTION), true));
var_dump(in_array($functions, spl_autoload_functions(SPL_AUTOLOAD_CLASS | SPL_AUTOLOAD_FUNCTION), true));
var_dump(count(spl_autoload_functions(SPL_AUTOLOAD_CLASS | SPL_AUTOLOAD_FUNCTION)) === 2);
?>
--EXPECTF--
class AClass
function AFunction
bool(true)
bool(true)
bool(true)
bool(true)

Fatal error: Uncaught Error: spl_autoload_functions() expects either ZEND_AUTOLOAD_CLASS or ZEND_AUTOLOAD_FUNCTION as the first argument in %s:%d
Stack trace:
#0 %s(%d): spl_autoload_functions(3)
#1 {main}
  thrown in %s on line %d
