--TEST--
Test idate() function : usage variation - Checking return of year(1 or 2 digits) format starting with zero and nonzero.
--FILE--
<?php
echo "*** Testing idate() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
date_default_timezone_set("Asia/Calcutta");
$format = 'y';

echo "\n-- Testing idate() function for 2 digit year having no zero as starting number --\n";
$timestamp = mktime(8, 8, 8, 8, 8, 1970);
var_dump( idate($format, $timestamp) );

echo "\n-- Testing idate() function for 2 digit year having zero as starting number --\n";
$timestamp = mktime(8, 8, 8, 8, 8, 2001);
var_dump( idate($format, $timestamp) );
?>
--EXPECT--
*** Testing idate() : usage variation ***

-- Testing idate() function for 2 digit year having no zero as starting number --
int(70)

-- Testing idate() function for 2 digit year having zero as starting number --
int(1)
