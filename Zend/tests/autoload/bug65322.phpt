--TEST--
Bug #65322: compile time errors won't trigger auto loading
--FILE--
<?php

spl_autoload_register(function($class) {
    var_dump($class);
    class X {}
});

set_error_handler(function($_, $msg, $file) {
    var_dump($msg, $file);
    new X;
});

/* This is just a particular example of a non-fatal compile-time error
 * If this breaks in future, just find another example and use it instead */
eval('class A { private function __invoke() { } }');

?>
--EXPECTF--
string(%d) "The magic method A::__invoke() must have public visibility"
string(%d) "%s(%d) : eval()'d code"
string(1) "X"
