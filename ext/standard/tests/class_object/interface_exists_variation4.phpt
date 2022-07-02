--TEST--
Test interface_exists() function : test autoload default value
--FILE--
<?php
echo "*** Testing interface_exists() : test autoload default value ***\n";

spl_autoload_register(function ($class_name) {
    require_once $class_name . '.inc';
});

var_dump(interface_exists("AutoInterface"));

echo "\nDONE\n";

?>
--EXPECT--
*** Testing interface_exists() : test autoload default value ***
bool(true)

DONE
