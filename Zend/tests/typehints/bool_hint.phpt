--TEST--
bool typehint
--FILE--
<?php
require 'common.inc';

function foo(bool $a) {
    var_dump($a); 
}
foo(1); // E_RECOVERABLE_ERROR
foo("1"); // E_RECOVERABLE_ERROR
foo(1.0); // E_RECOVERABLE_ERROR
foo(0); // E_RECOVERABLE_ERROR
foo("0"); // E_RECOVERABLE_ERROR
foo("1a"); // E_RECOVERABLE_ERROR
foo("a"); // E_RECOVERABLE_ERROR
foo(""); // E_RECOVERABLE_ERROR
foo(1.5); // E_RECOVERABLE_ERROR
foo(array()); // E_RECOVERABLE_ERROR
foo(new StdClass); // bool(true)
foo(new AThree); // bool(true)
foo(fopen("data:text/plain,foobar", "r")); // E_RECOVERABLE_ERROR
foo(true); // bool(true)
foo(false); // bool(false)
foo(NULL); // E_RECOVERABLE_ERROR
?>
--EXPECTF--
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type bool, int given, called in %s on line 7 and defined in %s on line 4
bool(true)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type bool, string given, called in %s on line 8 and defined in %s on line 4
bool(true)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type bool, float given, called in %s on line 9 and defined in %s on line 4
bool(true)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type bool, int given, called in %s on line 10 and defined in %s on line 4
bool(false)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type bool, string given, called in %s on line 11 and defined in %s on line 4
bool(false)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type bool, string given, called in %s on line 12 and defined in %s on line 4
bool(true)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type bool, string given, called in %s on line 13 and defined in %s on line 4
bool(true)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type bool, string given, called in %s on line 14 and defined in %s on line 4
bool(false)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type bool, float given, called in %s on line 15 and defined in %s on line 4
bool(true)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type bool, array given, called in %s on line 16 and defined in %s on line 4
bool(false)
bool(true)
bool(true)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type bool, resource given, called in %s on line 19 and defined in %s on line 4
bool(true)
bool(true)
bool(false)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type bool, null given, called in %s on line 22 and defined in %s on line 4
bool(false)