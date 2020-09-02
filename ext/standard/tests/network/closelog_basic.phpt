--TEST--
Test closelog() function : basic functionality
--FILE--
<?php
echo "*** Testing closelog() : basic functionality ***\n";

// Zero arguments
echo "\n-- Testing closelog() function with Zero arguments --\n";
var_dump( closelog() );
?>
--EXPECT--
*** Testing closelog() : basic functionality ***

-- Testing closelog() function with Zero arguments --
bool(true)
