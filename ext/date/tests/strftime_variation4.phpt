--TEST--
Test strftime() function : usage variation - Passing month related format strings to format argument.
--FILE--
<?php
echo "*** Testing strftime() : usage variation ***\n";

date_default_timezone_set("Asia/Calcutta");
// Initialise function arguments not being substituted (if any)
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

//array of values to iterate over
$inputs = array(
      'Abbreviated month name' => "%b",
      'Full month name' => "%B",
      'Month as decimal' => "%m",
);

// loop through each element of the array for timestamp

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( strftime($value) );
      var_dump( strftime($value, $timestamp) );
};

?>
--EXPECTF--
*** Testing strftime() : usage variation ***

--Abbreviated month name--
string(%d) "%s"
string(3) "Aug"

--Full month name--
string(%d) "%s"
string(6) "August"

--Month as decimal--
string(%d) "%d"
string(2) "08"
