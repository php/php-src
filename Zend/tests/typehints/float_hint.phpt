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
foo("1a"); // E_RECOVERABLE_ERROR
foo("a"); // E_RECOVERABLE_ERROR
foo(1.5); // float(1.5)
foo(array()); // E_RECOVERABLE_ERROR
foo(new StdClass); // Notice: Object of class stdClass could not be converted to float float(1)
foo(new AThree); // Notice: Object of class AThree could not be converted to float float(1)
foo(fopen("data:text/plain,foobar", "r")); // E_RECOVERABLE_ERROR
foo(true); // E_RECOVERABLE_ERROR
foo(false); // E_RECOVERABLE_ERROR
foo(null); // E_RECOVERABLE_ERROR
?>
--EXPECTF--
float(1)
float(1)
float(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type float, string given, called in %s on line 10 and defined in %s on line 4
float(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type float, string given, called in %s on line 11 and defined in %s on line 4
float(0)
float(1.5)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type float, array given, called in %s on line 13 and defined in %s on line 4
float(0)
E_NOTICE: Object of class stdClass could not be converted to double in %s on line 4
float(1)
E_NOTICE: Object of class AThree could not be converted to double in %s on line 4
float(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type float, resource given, called in %s on line 16 and defined in %s on line 4
float(%d)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type float, boolean given, called in %s on line 17 and defined in %s on line 4
float(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type float, boolean given, called in %s on line 18 and defined in %s on line 4
float(0)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type float, null given, called in %s on line 19 and defined in %s on line 4
float(0)