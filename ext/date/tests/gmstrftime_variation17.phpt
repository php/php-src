--TEST--
Test gmstrftime() function : usage variation - Checking day related formats which was not supported on Windows before vc14.
--FILE--
<?php
echo "*** Testing gmstrftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$timestamp = gmmktime(8, 8, 8, 8, 8, 2008);
setlocale(LC_ALL, "C");
date_default_timezone_set("Asia/Calcutta");

echo "\n-- Testing gmstrftime() function with Day of the month as decimal single digit format --\n";
$format = "%e";
var_dump( gmstrftime($format) );
var_dump( gmstrftime($format, $timestamp) );

?>
--EXPECTF--
*** Testing gmstrftime() : usage variation ***

-- Testing gmstrftime() function with Day of the month as decimal single digit format --
string(2) "%A%d"
string(2) " 8"
