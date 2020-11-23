--TEST--
Test interface_exists() function : autoloaded interface
--FILE--
<?php
echo "*** Testing interface_exists() : autoloaded interface ***\n";

spl_autoload_register(function ($class_name) {
    require_once $class_name . '.inc';
});

echo "\n-- no autoloading --\n";
var_dump(interface_exists("AutoInterface", false));

echo "\n-- with autoloading --\n";
var_dump(interface_exists("AutoInterface", true));

echo "\nDONE\n";

?>
--EXPECT--
*** Testing interface_exists() : autoloaded interface ***

-- no autoloading --
bool(false)

-- with autoloading --
bool(true)

DONE
