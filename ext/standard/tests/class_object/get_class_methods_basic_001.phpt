--TEST--
Test get_class_methods() function : basic functionality
--FILE--
<?php
/*
 * Test basic behaviour with existing class and non-existent class.
 */

echo "*** Testing get_class_methods() : basic functionality ***\n";

class C {
    function f() {}
    function g() {}
    function h() {}
}

echo "Argument is class name:\n";
var_dump( get_class_methods("C") );
echo "Argument is class instance:\n";
$c = new C;
var_dump( get_class_methods($c) );

class D {}
echo "Argument is name of class which has no methods:\n";
var_dump( get_class_methods("D") );

echo "Argument is non existent class:\n";
try {
    var_dump( get_class_methods("NonExistent") );
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done";
?>
--EXPECT--
*** Testing get_class_methods() : basic functionality ***
Argument is class name:
array(3) {
  [0]=>
  string(1) "f"
  [1]=>
  string(1) "g"
  [2]=>
  string(1) "h"
}
Argument is class instance:
array(3) {
  [0]=>
  string(1) "f"
  [1]=>
  string(1) "g"
  [2]=>
  string(1) "h"
}
Argument is name of class which has no methods:
array(0) {
}
Argument is non existent class:
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, string given
Done
