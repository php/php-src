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
foo(array()); // E_RECOVERABLE_ERROR
foo(new StdClass); // E_RECOVERABLE_ERROR
foo(fopen("data:text/plain,foobar", "r")); // E_RECOVERABLE_ERROR
?>
--EXPECTF--
string(1) "1"
string(1) "1"
string(1) "1"
string(2) "1a"
string(1) "a"
string(3) "1.5"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type string, array given, called in %s on line 13 and defined in %s on line 4
string(5) "Array"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type string, object given, called in %s on line 14 and defined in %s on line 4
string(6) "Object"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type string, resource given, called in %s on line 15 and defined in %s on line 4
string(%d) "%s"
