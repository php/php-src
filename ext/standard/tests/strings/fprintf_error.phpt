--TEST--
Test fprintf() function (errors)
--FILE--
<?php

/* Testing Error Conditions */
echo "*** Testing Error Conditions ***\n";

/* zero argument */
var_dump( fprintf() );

/* scalar argument */
var_dump( fprintf(3) );

/* NULL argument */
var_dump( fprintf(NULL) );

echo "Done\n";
?>
--EXPECTF--
*** Testing Error Conditions ***

Warning: Wrong parameter count for fprintf() in %sfprintf_error.php on line %d
NULL

Warning: Wrong parameter count for fprintf() in %sfprintf_error.php on line %d
NULL

Warning: Wrong parameter count for fprintf() in %sfprintf_error.php on line %d
NULL
Done
