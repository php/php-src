--TEST--
Test strftime() function : basic functionality
--FILE--
<?php
/* Prototype  : string strftime(string format [, int timestamp])
 * Description: Format a local time/date according to locale settings
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing strftime() : basic functionality ***\n";

date_default_timezone_set("Asia/Calcutta");
// Initialise all required variables
$format = '%b %d %Y %H:%M:%S';
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

// Calling strftime() with all possible arguments
var_dump( strftime($format, $timestamp) );

// Calling strftime() with mandatory arguments
var_dump( strftime($format) );

?>
===DONE===
--EXPECTF--
*** Testing strftime() : basic functionality ***
string(20) "Aug 08 2008 08:08:08"
string(%d) "%s %d %d %d:%d:%d"
===DONE===
