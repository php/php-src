--TEST--
set $value parameter variance
--FILE--
<?php

spl_autoload_register(function ($className) {
    echo "Autoloading $className\n";
});

include __DIR__ . "/set_value_parameter_type_variance_004.inc";

?>
--EXPECTF--
Autoloading X
Autoloading Y

Fatal error: Type of parameter $prop of hook Test::$prop::set must be compatible with property type in %s on line %d
