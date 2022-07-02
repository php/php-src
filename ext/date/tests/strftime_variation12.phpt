--TEST--
Test strftime() function : usage variation - Checking month related formats which are supported other than on Windows.
--SKIPIF--
<?php
if (strtoupper(substr(PHP_OS, 0, 3)) == 'WIN') {
    die("skip Test is not valid for Windows");
}
?>
--FILE--
<?php
echo "*** Testing strftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
setlocale(LC_ALL, "en_US");
date_default_timezone_set("Asia/Calcutta");
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

echo "\n-- Testing strftime() function with  Abbreviated month name format %h --\n";
$format = "%h";
var_dump( strftime($format) );
var_dump( strftime($format, $timestamp) );

?>
--EXPECTF--
*** Testing strftime() : usage variation ***

-- Testing strftime() function with  Abbreviated month name format %h --

Deprecated: Function strftime() is deprecated in %s on line %d
string(%d) "%s"

Deprecated: Function strftime() is deprecated in %s on line %d
string(3) "Aug"
