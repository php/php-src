--TEST--
Test strftime() function : usage variation - Checking week related formats which was not supported on Windows before VC14.
--FILE--
<?php
echo "*** Testing strftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
setlocale(LC_ALL, "C");
date_default_timezone_set("Asia/Calcutta");
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

//array of values to iterate over
$inputs = array(
      'The ISO 8601:1988 week number' => "%V",
      'Weekday as decimal' => "%u",
);

// loop through each element of the array for timestamp

foreach($inputs as $key =>$value) {
    echo "\n--$key--\n";
    var_dump( strftime($value) );
    var_dump( strftime($value, $timestamp) );
}

?>
--EXPECTF--
*** Testing strftime() : usage variation ***

--The ISO 8601:1988 week number--
string(2) "%d"
string(2) "32"

--Weekday as decimal--
string(1) "%d"
string(1) "5"
