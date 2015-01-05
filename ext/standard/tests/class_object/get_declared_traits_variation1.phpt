--TEST--
Test get_declared_traits() function : testing autoloaded traits
--FILE--
<?php
/* Prototype  : proto array get_declared_traits()
 * Description: Returns an array of all declared traits. 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */


echo "*** Testing get_declared_traits() : testing autoloaded traits ***\n";

function __autoload($trait_name) {
    require_once $trait_name . '.inc';
}

echo "\n-- before instance is declared --\n";
var_dump(in_array('AutoTrait', get_declared_traits()));

echo "\n-- after use is declared --\n";

class MyClass {
    use AutoTrait;
}

var_dump(in_array('AutoTrait', get_declared_traits()));

echo "\nDONE\n";

?>
--EXPECTF--
*** Testing get_declared_traits() : testing autoloaded traits ***

-- before instance is declared --
bool(false)

-- after use is declared --
bool(true)

DONE
