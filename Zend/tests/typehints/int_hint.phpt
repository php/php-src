--TEST--
int typehint
--FILE--
<?php
require 'common.inc';

function foo(int $a) {
    var_dump($a); 
}
foo(1); // int(1)
foo("1"); // int(1)
foo(1.0); // int(1)
foo("1a"); // Notice: A non well formed numeric value encountered int(1)
foo("a"); // E_RECOVERABLE_ERROR
foo(999999999999999999999999999999999999); // E_RECOVERABLE_ERROR (since it's not exactly representable by an int)
foo(1.5); // E_RECOVERABLE_ERROR
foo(array()); // E_RECOVERABLE_ERROR
foo(new StdClass); // Notice: Object of class stdClass could not be converted to int int(1)
foo(fopen("data:text/plain,foobar", "r")); // E_RECOVERABLE_ERROR
?>
--EXPECTF--
int(1)
int(1)
int(1)
E_NOTICE: A non well formed numeric value encountered in %s on line 4
int(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type int, string given, called in %s on line 11 and defined in %s on line 4
int(0)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type int, double given, called in %s on line 12 and defined in %s on line 4
int(0)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type int, double given, called in %s on line 13 and defined in %s on line 4
int(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type int, array given, called in %s on line 14 and defined in %s on line 4
int(0)
E_NOTICE: Object of class stdClass could not be converted to int in %s on line 4
int(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type int, resource given, called in %s on line 16 and defined in %s on line 4
int(%d)