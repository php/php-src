--TEST--
Test copy() function: error conditions
--FILE--
<?php
/* Prototype: bool copy ( string $source, string $dest );
 * Description: Makes a copy of the file source to dest.
 *              Returns TRUE on success or FALSE on failure.
 */

echo "*** Testing copy() function: error conditions --\n"; 
/* Invalid args */
var_dump( copy("/no/file", "file") );

/* No.of args less than expected */
var_dump( copy() );
var_dump( copy(__FILE__) );

/* No.of args less than expected */
var_dump( copy(__FILE__, "file1", "file1") );

echo "*** Done ***\n";
?>

--EXPECTF--
*** Testing copy() function: error conditions --

Warning: copy(/no/file): failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: Wrong parameter count for copy() in %s on line %d
NULL

Warning: Wrong parameter count for copy() in %s on line %d
NULL

Warning: Wrong parameter count for copy() in %s on line %d
NULL
*** Done ***
