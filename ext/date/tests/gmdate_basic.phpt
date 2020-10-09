--TEST--
Test gmdate() function : basic functionality
--FILE--
<?php
echo "*** Testing gmdate() : basic functionality ***\n";

// Initialise all required variables
date_default_timezone_set('UTC');
$format = DATE_ISO8601;
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

// Calling gmdate() with all possible arguments
var_dump( gmdate($format, $timestamp) );

// Calling gmdate() with mandatory arguments
var_dump( gmdate($format) );

?>
--EXPECTF--
*** Testing gmdate() : basic functionality ***
string(24) "2008-08-08T08:08:08+0000"
string(%d) "%s"
