--TEST--
Test idate() function : basic functionality 
--FILE--
<?php
/* Prototype  : int idate(string format [, int timestamp])
 * Description: Format a local time/date as integer 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing idate() : basic functionality ***\n";

// Initialise all required variables
$format = 'Y';

// Calling idate() with mandatory arguments
date_default_timezone_set("Asia/Calcutta");
var_dump( idate($format) );
?>
===DONE===
--EXPECTF--
*** Testing idate() : basic functionality ***
int(%d)
===DONE===
