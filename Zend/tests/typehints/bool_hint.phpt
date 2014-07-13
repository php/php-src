--TEST--
bool typehint
--FILE--
<?php
require 'common.inc';

function foo(bool $a) {
    var_dump($a); 
}
foo(1); // bool(true)
foo("1"); // bool(true)
foo(1.0); // bool(true)
foo(0); // bool(false)
foo("0"); // bool(false)
foo("1a"); // bool(true)
foo("a"); // bool(true)
foo(1.5); // bool(true)
foo(array()); // E_RECOVERABLE_ERROR
foo(new StdClass); // bool(true)
foo(new AThree); // bool(true)
foo(fopen("data:text/plain,foobar", "r")); // E_RECOVERABLE_ERROR
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type boolean, array given, called in %s on line 15 and defined in %s on line 4
bool(false)
bool(true)
bool(true)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type boolean, resource given, called in %s on line 18 and defined in %s on line 4
bool(true)