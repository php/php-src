--TEST--
Basic autoload_call_class() function
--CREDITS--
Jean-Marc Fontaine <jean-marc.fontaine@alterway.fr>
# Alter Way Contribution Day 2011
--FILE--
<?php
function customAutoloader($class) {
    class TestClass {}
}
autoload_register_class('customAutoloader');

autoload_call_class('TestClass');
var_dump(class_exists('TestClass', false));
?>
--EXPECT--
bool(true)
