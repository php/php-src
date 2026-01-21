--TEST--
spl_autoload_call() function - basic test for spl_autoload_call()
--CREDITS--
Jean-Marc Fontaine <jean-marc.fontaine@alterway.fr>
# Alter Way Contribution Day 2011
--FILE--
<?php
function customAutolader($class) {
    require_once __DIR__ . '/testclass.class.inc';
}
spl_autoload_register('customAutolader');

spl_autoload_call('TestClass');
var_dump(class_exists('TestClass', false));
?>
--EXPECTF--
%stestclass.class.inc
bool(true)
