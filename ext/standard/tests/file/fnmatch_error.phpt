--TEST--
Test fnmatch() function: Error conditions
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == 'WIN')
  die("skip do not run on Windows");
?>
--FILE--
<?php
/* Prototype: bool fnmatch ( string $pattern, string $string [, int $flags] )
   Description: fnmatch() checks if the passed string would match 
     the given shell wildcard pattern. 
*/

echo "*** Testing error conditions for fnmatch() ***";

/* Invalid arguments */
var_dump( fnmatch(array(), array()) );

$file_handle = fopen(__FILE__, "r");
var_dump( fnmatch($file_handle, $file_handle) );
fclose( $file_handle );

$std_obj = new stdClass();
var_dump( fnmatch($std_obj, $std_obj) );


/* No.of arguments less than expected */
var_dump( fnmatch("match.txt") );
var_dump( fnmatch("") );

/* No.of arguments greater than expected */
var_dump( fnmatch("match.txt", "match.txt", TRUE, 100) );

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing error conditions for fnmatch() ***
Warning: fnmatch() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL

Warning: fnmatch() expects parameter 1 to be string (Unicode or binary), resource given in %s on line %d
NULL

Warning: fnmatch() expects parameter 1 to be string (Unicode or binary), object given in %s on line %d
NULL

Warning: fnmatch() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: fnmatch() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: fnmatch() expects at most 3 parameters, 4 given in %s on line %d
NULL

*** Done ***
