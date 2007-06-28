--TEST--
Test array_search() and in_array() functions (errors)
--FILE--
<?php

echo "\n*** Testing error conditions of in_array() ***\n";
/* zero argument */
var_dump( in_array() );

/* unexpected no.of arguments in in_array() */
$var = array("mon", "tues", "wed", "thurs");
var_dump( in_array(1, $var, 0, "test") );
var_dump( in_array("test") );

/* unexpected second argument in in_array() */
$var="test";
var_dump( in_array("test", $var) );
var_dump( in_array(1, 123) );

echo "Done\n";
?>
--EXPECTF--
*** Testing error conditions of in_array() ***

Warning: Wrong parameter count for in_array() in %s on line %d
NULL

Warning: Wrong parameter count for in_array() in %s on line %d
NULL

Warning: Wrong parameter count for in_array() in %s on line %d
NULL

Warning: in_array(): Wrong datatype for second argument in %s on line %d
bool(false)

Warning: in_array(): Wrong datatype for second argument in %s on line %d
bool(false)
Done
