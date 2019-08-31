--TEST--
Test strftime() function : usage variation - Checking day related formats which was not supported on Windows before VC14.
--FILE--
<?php
/* Prototype  : string strftime(string format [, int timestamp])
 * Description: Format a local time/date according to locale settings
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing strftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
setlocale(LC_ALL, "C");
date_default_timezone_set("Asia/Calcutta");
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

echo "\n-- Testing strftime() function with Day of the month as decimal single digit format --\n";
$format = "%e";
var_dump( strftime($format) );
var_dump( strftime($format, $timestamp) );
?>
===DONE===
--EXPECTF--
*** Testing strftime() : usage variation ***

-- Testing strftime() function with Day of the month as decimal single digit format --
string(%d) "%A%d"
string(2) " 8"
===DONE===
