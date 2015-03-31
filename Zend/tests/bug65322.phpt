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

/* This is just a particular example of a non-fatal compile-time error
 * If this breaks in future, just find another example and use it instead */
eval('abstract class foo { abstract static function bar(); }');

?>
--EXPECTF--
string(%d) "Static function foo::bar() should not be abstract"
string(%d) "%s(%d) : eval()'d code"
string(1) "B"
