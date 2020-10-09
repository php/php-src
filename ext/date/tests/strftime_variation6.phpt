--TEST--
Test strftime() function : usage variation - Passing time related format strings to format argument.
--FILE--
<?php
echo "*** Testing strftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
setlocale(LC_ALL, "en_US");
date_default_timezone_set("Asia/Calcutta");
$timestamp = mktime(18, 8, 8, 8, 8, 2008);


//array of values to iterate over
$inputs = array(
      'Hour as decimal by 24-hour format' => "%H",
      'Hour as decimal by 12-hour format' => "%I",
      'Minute as decimal number' => "%M",
      'AM/PM format for a time' => "%p",
      'Second as decimal number' => "%S",
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

--Hour as decimal by 24-hour format--
string(%d) "%d"
string(2) "18"

--Hour as decimal by 12-hour format--
string(%d) "%d"
string(2) "06"

--Minute as decimal number--
string(%d) "%d"
string(2) "08"

--AM/PM format for a time--
string(%d) "%s"
string(2) "PM"

--Second as decimal number--
string(%d) "%d"
string(2) "08"
