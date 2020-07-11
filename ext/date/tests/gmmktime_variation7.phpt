--TEST--
Test gmmktime() function : usage variation - Checking with few optional arguments.
--FILE--
<?php
echo "*** Testing gmmktime() : usage variation ***\n";

// Initialise all required variables
$hour = 8;
$min = 8;
$sec = 8;
$mon = 8;
$day = 8;

echo "\n-- Testing gmmktime() function with one optional argument --\n";
var_dump( gmmktime($hour) );

echo "\n-- Testing gmmktime() function with two optional argument --\n";
var_dump( gmmktime($hour, $min) );

echo "\n-- Testing gmmktime() function with three optional argument --\n";
var_dump( gmmktime($hour, $min, $sec) );

echo "\n-- Testing gmmktime() function with four optional argument --\n";
var_dump( gmmktime($hour, $min, $sec, $mon) );

echo "\n-- Testing gmmktime() function with five optional argument --\n";
var_dump( gmmktime($hour, $min, $sec, $mon, $day) );

?>
--EXPECTF--
*** Testing gmmktime() : usage variation ***

-- Testing gmmktime() function with one optional argument --
int(%d)

-- Testing gmmktime() function with two optional argument --
int(%d)

-- Testing gmmktime() function with three optional argument --
int(%d)

-- Testing gmmktime() function with four optional argument --
int(%d)

-- Testing gmmktime() function with five optional argument --
int(%d)
