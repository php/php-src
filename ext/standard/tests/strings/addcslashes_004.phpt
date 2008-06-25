--TEST--
Test addcslashes() function (errors)
--INI--
precision=14
--FILE--
<?php

echo "\n*** Testing error conditions ***\n";
/* zero argument */
var_dump( addcslashes() );

/* unexpected arguments */
var_dump( addcslashes("foo[]") );
var_dump( addcslashes('foo[]', "o", "foo") );

echo "Done\n"; 

?>
--EXPECTF--
*** Testing error conditions ***

Warning: addcslashes() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: addcslashes() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: addcslashes() expects exactly 2 parameters, 3 given in %s on line %d
NULL
Done
