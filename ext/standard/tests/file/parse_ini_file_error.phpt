--TEST--
Test parse_ini_file() function : error conditions
--FILE--
<?php
echo "*** Testing parse_ini_file() : error conditions ***\n";

//Test parse_ini_file with one more than the expected number of arguments
echo "\n-- Testing parse_ini_file() function with invalid scanner mode --\n";
$filename = 'string_val';
$process_sections = true;
$scanner_mode = 10;
var_dump( parse_ini_file($filename, $process_sections, $scanner_mode) );

echo "\n-- Testing parse_ini_file() function with a non-existent file --\n";
$filename = __FILE__ . 'invalidfilename';
var_dump( parse_ini_file($filename, $process_sections) );

echo "Done";
?>
--EXPECTF--
*** Testing parse_ini_file() : error conditions ***

-- Testing parse_ini_file() function with invalid scanner mode --

Warning: parse_ini_file(): Invalid scanner mode in %s on line %d
bool(false)

-- Testing parse_ini_file() function with a non-existent file --

Warning: parse_ini_file(): Failed to open stream: No such file or directory in %s on line %d
bool(false)
Done
