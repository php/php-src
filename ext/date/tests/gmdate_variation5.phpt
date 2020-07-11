--TEST--
Test gmdate() function : usage variation - Passing Week representation to format.
--FILE--
<?php
echo "*** Testing gmdate() : usage variation ***\n";

// Initialise all required variables
date_default_timezone_set('UTC');
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

echo "\n-- Testing gmdate() function with ISO-8601 week number of year format --\n";
var_dump( gmdate('W') );
var_dump( gmdate('W', $timestamp) );

?>
--EXPECTF--
*** Testing gmdate() : usage variation ***

-- Testing gmdate() function with ISO-8601 week number of year format --
string(%d) "%d"
string(2) "32"
