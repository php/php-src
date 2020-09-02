--TEST--
Test idate() function : basic functionality
--FILE--
<?php
echo "*** Testing idate() : basic functionality ***\n";

// Initialise all required variables
$format = 'Y';

// Calling idate() with mandatory arguments
date_default_timezone_set("Asia/Calcutta");
var_dump( idate($format) );
?>
--EXPECTF--
*** Testing idate() : basic functionality ***
int(%d)
