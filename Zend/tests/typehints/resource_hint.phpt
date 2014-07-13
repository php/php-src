--TEST--
resource typehint
--FILE--
<?php
require 'common.inc';

function foo(resource $a) {
    var_dump($a); 
}
foo(1); // E_RECOVERABLE_ERROR
foo("1"); // E_RECOVERABLE_ERROR
foo(1.0); // E_RECOVERABLE_ERROR
foo(0); // E_RECOVERABLE_ERROR
foo("0"); // E_RECOVERABLE_ERROR
foo("1a"); // E_RECOVERABLE_ERROR
foo("a"); // E_RECOVERABLE_ERROR
foo(1.5); // E_RECOVERABLE_ERROR
foo(array()); // E_RECOVERABLE_ERROR
foo(new StdClass); // E_RECOVERABLE_ERROR
foo(new AThree); // E_RECOVERABLE_ERROR
foo(fopen("data:text/plain,foobar", "r")); // works
?>
--EXPECTF--
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, integer given, called in %s on line 7 and defined in %s on line 4
int(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, string given, called in %s on line 8 and defined in %s on line 4
string(1) "1"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, double given, called in %s on line 9 and defined in %s on line 4
float(1)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, integer given, called in %s on line 10 and defined in %s on line 4
int(0)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, string given, called in %s on line 11 and defined in %s on line 4
string(1) "0"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, string given, called in %s on line 12 and defined in %s on line 4
string(2) "1a"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, string given, called in %s on line 13 and defined in %s on line 4
string(1) "a"
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, double given, called in %s on line 14 and defined in %s on line 4
float(1.5)
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, array given, called in %s on line 15 and defined in %s on line 4
array(0) {
}
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, object given, called in %s on line 16 and defined in %s on line 4
object(stdClass)#%d (0) {
}
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be of the type resource, object given, called in %s on line 17 and defined in %s on line 4
object(AThree)#%d (0) {
}
resource(%d) of type (stream)