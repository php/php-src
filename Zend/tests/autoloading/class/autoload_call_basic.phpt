--TEST--
autoload_call_class() function - basic test for spl_autoload_call()
--CREDITS--
Jean-Marc Fontaine <jean-marc.fontaine@alterway.fr>
# Alter Way Contribution Day 2011
--FILE--
<?php
function customAutolader($class) {
    class TestClass {}
}
autoload_register_class('customAutolader');

autoload_call_class('TestClass');
var_dump(class_exists('TestClass', false));
?>
--EXPECT--
bool(true)
