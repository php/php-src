--TEST--
Bug #65322: compile time errors won't trigger auto loading
--FILE--
<?php

spl_autoload_register(function($class) {
    var_dump($class);
    class B {}
});

set_error_handler(function($_, $msg, $file) {
    var_dump($msg, $file);
    new B;
});

eval('class A { function a() {} function __construct() {} }');

?>
--EXPECTF--
string(50) "Redefining already defined constructor for class A"
string(%d) "%s(%d) : eval()'d code"
string(1) "B"
