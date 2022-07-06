--TEST--
Test join() function : basic functionality
--FILE--
<?php
echo "*** Testing join() : basic functionality ***\n";

// Initialize all required variables
$glue = ',';
$pieces = array(1, 2, 3, 4);

// pieces as array with numeric values
var_dump( join($glue, $pieces) );

// pieces as array with strings values
$glue = ", "; // multiple car as glue
$pieces = array("Red", "Green", "Blue", "Black", "White");
var_dump( join($glue, $pieces) );

// pices as associative array (numeric values)
$pieces = array("Hour" => 10, "Minute" => 20, "Second" => 40);
$glue = ':';
var_dump( join($glue, $pieces) );

// pices as associative array (string/numeric values)
$pieces = array("Day" => 'Friday', "Month" => "September", "Year" => 2007);
$glue = '/';
var_dump( join($glue, $pieces) );

echo "Done\n";
?>
--EXPECT--
*** Testing join() : basic functionality ***
string(7) "1,2,3,4"
string(30) "Red, Green, Blue, Black, White"
string(8) "10:20:40"
string(21) "Friday/September/2007"
Done
