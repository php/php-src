--TEST--
spl_autoload_register() function - warn when using false as second argument for spl_autoload_register()
--FILE--
<?php
function customAutolader($class) {
    require_once __DIR__ . '/testclass.class.inc';
}
spl_autoload_register('customAutolader', false);

spl_autoload_call('TestClass');
var_dump(class_exists('TestClass', false));
?>
--EXPECTF--
Notice: spl_autoload_register(): Argument #2 ($do_throw) has been ignored, spl_autoload_register() will always throw in %s on line %d
%stestclass.class.inc
bool(true)
