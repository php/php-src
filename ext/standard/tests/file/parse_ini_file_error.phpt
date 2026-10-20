--TEST--
Test parse_ini_file() function : error conditions
--FILE--
<?php
echo "*** Testing parse_ini_file() : error conditions ***\n";

//Test parse_ini_file with one more than the expected number of arguments
echo "\n-- Testing parse_ini_file() function with invalid scanner mode --\n";
$filename = 'string_val';
$process_sections = true;

try {
	$scanner_mode = 10;
	var_dump( parse_ini_file($filename, $process_sections, $scanner_mode) );
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n-- Testing parse_ini_file() function with a non-existent file --\n";
$filename = __FILE__ . 'invalidfilename';
var_dump( parse_ini_file($filename, $process_sections) );

echo "Done";
?>
--EXPECTF--
*** Testing parse_ini_file() : error conditions ***

-- Testing parse_ini_file() function with invalid scanner mode --
ValueError: parse_ini_file(): Argument #3 ($scanner_mode) must be one of INI_SCANNER_NORMAL, INI_SCANNER_RAW, or INI_SCANNER_TYPED

-- Testing parse_ini_file() function with a non-existent file --

Warning: parse_ini_file(): Failed to open stream: No such file or directory in %s on line %d
bool(false)
Done
