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
/* Prototype  : string gmstrftime(string format [, int timestamp])
 * Description: Format a GMT/UCT time/date according to locale settings 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

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
===DONE===
--EXPECTF--
*** Testing gmstrftime() : usage variation ***

-- Testing gmstrftime() function with Day of the month as decimal single digit format --
string(%d) "%s"
string(2) " 8"
===DONE===
