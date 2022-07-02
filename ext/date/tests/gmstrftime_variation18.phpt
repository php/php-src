--TEST--
Test gmstrftime() function : usage variation - Checking day related formats which are supported other than on Windows.
--SKIPIF--
<?php
if (strtoupper(substr(PHP_OS, 0, 3)) == 'WIN') {
    die("skip Test is not valid for Windows");
}
?>
--FILE--
<?php
echo "*** Testing gmstrftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$timestamp = gmmktime(8, 8, 8, 8, 8, 2008);
setlocale(LC_ALL, "en_US");
date_default_timezone_set("Asia/Calcutta");

echo "\n-- Testing gmstrftime() function with Day of the month as decimal single digit format --\n";
$format = "%e";
var_dump( gmstrftime($format) );
var_dump( gmstrftime($format, $timestamp) );

?>
--EXPECTF--
*** Testing gmstrftime() : usage variation ***

-- Testing gmstrftime() function with Day of the month as decimal single digit format --

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(%d) "%s"

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(2) " 8"
