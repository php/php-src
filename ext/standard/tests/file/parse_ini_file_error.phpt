--TEST--
Test parse_ini_file() function : error conditions 
--FILE--
<?php
/* Prototype  : proto array parse_ini_file(string filename [, bool process_sections])
 * Description: Parse configuration file 
 * Source code: ext/standard/basic_functions.c
 * Alias to functions: 
 */

/*
 * add a comment here to say what the test is supposed to do
 */

echo "*** Testing parse_ini_file() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing parse_ini_file() function with Zero arguments --\n";
var_dump( parse_ini_file() );

//Test parse_ini_file with one more than the expected number of arguments
echo "\n-- Testing parse_ini_file() function with more than expected no. of arguments --\n";
$filename = 'string_val';
$process_sections = true;
$extra_arg = 10;
var_dump( parse_ini_file($filename, $process_sections, $extra_arg) );

echo "\n-- Testing parse_ini_file() function with a non-existent file --\n";
$filename = __FILE__ . 'invalidfilename';
var_dump( parse_ini_file($filename, $process_sections) ); 

echo "Done";
?>
--EXPECTF--
*** Testing parse_ini_file() : error conditions ***

-- Testing parse_ini_file() function with Zero arguments --

Warning: Wrong parameter count for parse_ini_file() in %s on line %d
NULL

-- Testing parse_ini_file() function with more than expected no. of arguments --

Warning: Wrong parameter count for parse_ini_file() in %s on line %d
NULL

-- Testing parse_ini_file() function with a non-existent file --

Warning: parse_ini_file(%s): failed to open stream: No such file or directory in %s on line %d
array(0) {
}
Done
