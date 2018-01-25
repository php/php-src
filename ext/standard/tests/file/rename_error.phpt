--TEST--
Test rename() function: error conditions
--FILE--
<?php
/* Prototype: bool rename ( string $oldname, string $newname [, resource $context] );
   Description: Renames a file or directory
*/

echo "*** Testing rename() for error conditions ***\n";
//Zero argument
var_dump( rename() );

// less than expected,1 argument
var_dump( rename(__FILE__) );

// more than expected no. of arguments
$context = stream_context_create();
$filename = __FILE__;
$new_filename = __FILE__.".tmp";
var_dump( rename($filename, $new_filename, $context, "extra_args") );

echo "Done\n";
?>
--EXPECTF--
*** Testing rename() for error conditions ***

Warning: rename() expects at least 2 parameters, 0 given in %s on line %d
bool(false)

Warning: rename() expects at least 2 parameters, 1 given in %s on line %d
bool(false)

Warning: rename() expects at most 3 parameters, 4 given in %s on line %d
bool(false)
Done
