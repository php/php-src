--TEST--
Test gmstrftime() function : usage variation - Checking week related formats which was not supported on Windows before vc14.
--FILE--
<?php
echo "*** Testing gmstrftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$timestamp = gmmktime(8, 8, 8, 8, 8, 2008);
setlocale(LC_ALL, "C");
date_default_timezone_set("Asia/Calcutta");

//array of values to iterate over
$inputs = array(
      'The ISO 8601:1988 week number' => "%V",
      'Weekday as decimal' => "%u",
);

// loop through each element of the array for timestamp

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( gmstrftime($value) );
      var_dump( gmstrftime($value, $timestamp) );
};

?>
--EXPECTF--
*** Testing gmstrftime() : usage variation ***

--The ISO 8601:1988 week number--
string(%d) "%d"
string(2) "32"

--Weekday as decimal--
string(1) "%d"
string(1) "5"
