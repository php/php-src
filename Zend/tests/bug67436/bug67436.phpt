--TEST--
bug67436: Autoloader isn't called if user defined error handler is present
--INI--
error_reporting=-1
--SKIPIF--
<?php if (extension_loaded('Zend OPCache')) die('skip Opcache overrides error handler'); ?>
--FILE--
<?php

spl_autoload_register(function($classname) {
    if (in_array($classname, array('a','b','c'))) {
        require_once __DIR__ . "/{$classname}.inc";
    }
});

set_error_handler(function ($errno, $errstr, $errfile, $errline) {
    var_dump($errstr);
}, error_reporting());

a::staticTest();

$b = new b();
$b->test();
?>
--EXPECTF--
string(%d) "The magic method b::__invoke() must have public visibility"
b::test()
a::test(c::TESTCONSTANT)
