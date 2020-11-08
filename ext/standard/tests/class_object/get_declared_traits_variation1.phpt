--TEST--
Test get_declared_traits() function : testing autoloaded traits
--FILE--
<?php
echo "*** Testing get_declared_traits() : testing autoloaded traits ***\n";

spl_autoload_register(function ($trait_name) {
    require_once $trait_name . '.inc';
});

echo "\n-- before instance is declared --\n";
var_dump(in_array('AutoTrait', get_declared_traits()));

echo "\n-- after use is declared --\n";

class MyClass {
    use AutoTrait;
}

var_dump(in_array('AutoTrait', get_declared_traits()));

echo "\nDONE\n";

?>
--EXPECT--
*** Testing get_declared_traits() : testing autoloaded traits ***

-- before instance is declared --
bool(false)

-- after use is declared --
bool(true)

DONE
