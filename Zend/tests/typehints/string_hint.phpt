--TEST--
string typehint
--FILE--
<?php
require 'common.inc';

function foo(string $a) {
    var_dump($a); 
}
foo(1); // string "1"
foo("1"); // string "1"
foo(1.0); // string "1"
foo("1a"); // string "1a"
foo("a"); // string "a"
foo(1.5); // string "1.5"
foo(""); // string ""
foo(array()); // E_RECOVERABLE_ERROR
foo(new StdClass); // E_RECOVERABLE_ERROR
foo(new AThree); // string "a3"
foo(fopen("data:text/plain,foobar", "r")); // E_RECOVERABLE_ERROR
foo(true); // E_RECOVERABLE_ERROR
foo(false); // E_RECOVERABLE_ERROR
foo(null); // E_RECOVERABLE_ERROR
?>
--EXPECTF--
string(1) "1"
string(1) "1"
string(1) "1"
string(2) "1a"
string(1) "a"
string(3) "1.5"
string(0) ""
E_NOTICE: Array to string conversion in %s on line 4
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type string, array given, called in %s on line 14 and defined in %s on line 4
string(5) "Array"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type string, object given, called in %s on line 15 and defined in %s on line 4
string(6) "Object"
string(2) "a3"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type string, resource given, called in %s on line 17 and defined in %s on line 4
string(%d) "%s"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type string, bool given, called in %s on line 18 and defined in %s on line 4
string(1) "1"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type string, bool given, called in %s on line 19 and defined in %s on line 4
string(0) ""
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type string, null given, called in %s on line 20 and defined in %s on line 4
string(0) ""