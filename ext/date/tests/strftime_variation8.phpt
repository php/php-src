--TEST--
Test strftime() function : usage variation - Passing literal related strings to format argument.
--FILE--
<?php
echo "*** Testing strftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
setlocale(LC_ALL, "en_US");
date_default_timezone_set("Asia/Calcutta");
$timestamp = mktime(8, 8, 8, 8, 8, 2008);
$format = "%%";

echo "\n-- Testing strftime() function with a literal % character to format --\n";
var_dump( strftime($format) );
var_dump( strftime($format, $timestamp) );

?>
--EXPECTF--
*** Testing strftime() : usage variation ***

-- Testing strftime() function with a literal % character to format --

Deprecated: Function strftime() is deprecated in %s on line %d
string(1) "%"

Deprecated: Function strftime() is deprecated in %s on line %d
string(1) "%"
