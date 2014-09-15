--TEST--
numeric typehint
--FILE--
<?php
require 'common.inc';

function foo(numeric $a) {
    var_dump($a); 
}
foo(1); // int(1)
foo("1"); // int(1)
foo(1.0); // float(1)
foo("1a"); // E_RECOVERABLE_ERROR
foo("a"); // E_RECOVERABLE_ERROR
foo(""); // E_RECOVERABLE_ERROR
foo(999999999999999999999999999999999999); // float(1.0E+36)
foo(1.5); // E_RECOVERABLE_ERROR
foo(array()); // E_RECOVERABLE_ERROR
foo(new StdClass); // Notice: Object of class stdClass could not be converted to int int(1)
foo(new AThree); // Notice: Object of class AThree could not be converted to int int(1)
foo(fopen("data:text/plain,foobar", "r")); // E_RECOVERABLE_ERROR
foo(true); // E_RECOVERABLE_ERROR
foo(false); // E_RECOVERABLE_ERROR
foo(null); // E_RECOVERABLE_ERROR
?>
--EXPECTF--
int(1)
int(1)
float(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of a numeric type, string given, called in %s on line 10 and defined in %s on line 4
int(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of a numeric type, string given, called in %s on line 11 and defined in %s on line 4
int(0)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of a numeric type, string given, called in %s on line 12 and defined in %s on line 4
int(0)
float(1.0E+36)
float(1.5)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of a numeric type, array given, called in %s on line 15 and defined in %s on line 4
int(0)
E_NOTICE: Object of class stdClass could not be converted to int in %s on line 4
int(1)
E_NOTICE: Object of class AThree could not be converted to int in %s on line 4
int(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of a numeric type, resource given, called in %s on line 18 and defined in %s on line 4
int(%d)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of a numeric type, bool given, called in %s on line 19 and defined in %s on line 4
int(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of a numeric type, bool given, called in %s on line 20 and defined in %s on line 4
int(0)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of a numeric type, null given, called in %s on line 21 and defined in %s on line 4
int(0)