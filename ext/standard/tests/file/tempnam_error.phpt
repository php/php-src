--TEST--
Test tempnam() function: error conditions
--FILE--
<?php
/* Prototype:  string tempnam ( string $dir, string $prefix );
   Description: Create file with unique file name.
*/

echo "*** Testing tempnam() error conditions ***\n";
$file_path = dirname(__FILE__);

/* More number of arguments than expected */
var_dump( tempnam("$file_path", "tempnam_error.tmp", "") ); //Two Valid & One Invalid
var_dump( tempnam("$file_path", "tempnam_error.tmp", TRUE) );

/* Less number of arguments than expected */
var_dump( tempnam("tempnam_error") );  //One Valid arg
var_dump( tempnam("$file_path") );  //One Valid arg
var_dump( tempnam("") );  //Empty string
var_dump( tempnam(NULL) );  //NULL as arg
var_dump( tempnam() );  //Zero args

?>
--EXPECTF--
*** Testing tempnam() error conditions ***

Warning: Wrong parameter count for tempnam() in %s on line %d
NULL

Warning: Wrong parameter count for tempnam() in %s on line %d
NULL

Warning: Wrong parameter count for tempnam() in %s on line %d
NULL

Warning: Wrong parameter count for tempnam() in %s on line %d
NULL

Warning: Wrong parameter count for tempnam() in %s on line %d
NULL

Warning: Wrong parameter count for tempnam() in %s on line %d
NULL

Warning: Wrong parameter count for tempnam() in %s on line %d
NULL
