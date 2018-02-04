--TEST--
Test readfile() function: error conditions
--FILE--
<?php
/* Prototype: int readfile ( string $filename [, bool $use_include_path [, resource $context]] );
   Description: Outputs a file
*/

$context = stream_context_create();

echo "*** Test readfile(): error conditions ***\n";
echo "-- Testing readfile() with unexpected no. of arguments --\n";
var_dump( readfile() );  // args < expected
var_dump( readfile(__FILE__, true, $context, 4) );  // args > expected

echo "\n-- Testing readfile() with invalid arguments --\n";
// invalid arguments
var_dump( readfile(NULL) );  // NULL as $filename
var_dump( readfile('') );  // empty string as $filename
var_dump( readfile(false) );  // boolean false as $filename
var_dump( readfile(__FILE__, false, '') );  // empty string as $context
var_dump( readfile(__FILE__, true, false) );  // boolean false as $context

echo "\n-- Testing readfile() with non-existent file --\n";
$non_existent_file = dirname(__FILE__)."/non_existent_file.tmp";
var_dump( readfile($non_existent_file) );

echo "Done\n";
?>
--EXPECTF--
*** Test readfile(): error conditions ***
-- Testing readfile() with unexpected no. of arguments --

Warning: readfile() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

Warning: readfile() expects at most 3 parameters, 4 given in %s on line %d
bool(false)

-- Testing readfile() with invalid arguments --

Warning: readfile(): Filename cannot be empty in %s on line %d
bool(false)

Warning: readfile(): Filename cannot be empty in %s on line %d
bool(false)

Warning: readfile(): Filename cannot be empty in %s on line %d
bool(false)

Warning: readfile() expects parameter 3 to be resource, string given in %s on line %d
bool(false)

Warning: readfile() expects parameter 3 to be resource, bool given in %s on line %d
bool(false)

-- Testing readfile() with non-existent file --

Warning: readfile(%s/non_existent_file.tmp): failed to open stream: %s in %s on line %d
bool(false)
Done
