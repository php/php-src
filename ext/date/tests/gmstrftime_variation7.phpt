--TEST--
Test gmstrftime() function : usage variation - Passing day related format strings to format argument.
--FILE--
<?php
echo "*** Testing gmstrftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$timestamp = gmmktime(8, 8, 8, 8, 8, 2008);
setlocale(LC_ALL, "en_US");
date_default_timezone_set("Asia/Calcutta");

//array of values to iterate over
$inputs = array(
      'Day of the month as a decimal number' => "%d",
      'Day of the year as a decimal number' => "%j",
      'Day of the week as a decimal number' => "%w"
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

--Day of the month as a decimal number--
string(%d) "%d"
string(2) "08"

--Day of the year as a decimal number--
string(%d) "%d"
string(3) "221"

--Day of the week as a decimal number--
string(%d) "%d"
string(1) "5"
