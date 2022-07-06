--TEST--
bug67436: E_WARNING instead of custom error handler
--FILE--
<?php

spl_autoload_register(function($classname) {
    if (in_array($classname, array('a','b','c'))) {
        require_once __DIR__ . "/{$classname}.inc";
    }
});

a::staticTest();

$b = new b();
$b->test();
?>
--EXPECTF--
Warning: The magic method b::__invoke() must have public visibility in %s on line %d
b::test()
a::test(c::TESTCONSTANT)
