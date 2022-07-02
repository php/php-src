--TEST--
Test strftime() function : basic functionality
--FILE--
<?php
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
--EXPECTF--
*** Testing strftime() : basic functionality ***

Deprecated: Function strftime() is deprecated in %s on line %d
string(20) "Aug 08 2008 08:08:08"

Deprecated: Function strftime() is deprecated in %s on line %d
string(%d) "%s %d %d %d:%d:%d"
