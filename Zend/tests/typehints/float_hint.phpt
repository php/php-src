--TEST--
float typehint
--FILE--
<?php
require 'common.inc';

function foo(float $a) {
    var_dump($a); 
}
foo(1); // float(1)
foo("1"); // float(1)
foo(1.0); // float(1)
foo("1a"); // Notice: A non well formed numeric value encountered float(1)
foo("a"); // E_RECOVERABLE_ERROR
foo(1.5); // float(1.5)
foo(array()); // E_RECOVERABLE_ERROR
foo(new StdClass); // Notice: Object of class stdClass could not be converted to float float(1)
foo(fopen("data:text/plain,foobar", "r")); // E_RECOVERABLE_ERROR
?>
--EXPECTF--
float(1)
float(1)
float(1)
E_NOTICE: A non well formed numeric value encountered in %s on line 4
float(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type float, string given, called in %s on line 11 and defined in %s on line 4
int(0)
float(1.5)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type float, array given, called in %s on line 13 and defined in %s on line 4
float(0)
E_NOTICE: Object of class stdClass could not be converted to double in %s on line 4
float(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type float, resource given, called in %s on line 15 and defined in %s on line 4
float(%d)
