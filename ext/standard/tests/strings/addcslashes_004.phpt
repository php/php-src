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

Warning: Wrong parameter count for addcslashes() in %s on line %d
NULL

Warning: Wrong parameter count for addcslashes() in %s on line %d
NULL

Warning: Wrong parameter count for addcslashes() in %s on line %d
NULL
Done
