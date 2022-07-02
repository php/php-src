--TEST--
Test gmstrftime() function : usage variation - Passing time related format strings to format argument.
--FILE--
<?php
echo "*** Testing gmstrftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$timestamp = gmmktime(8, 8, 8, 8, 8, 2008);
setlocale(LC_ALL, "en_US");
date_default_timezone_set("Asia/Calcutta");

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
      var_dump( gmstrftime($value) );
      var_dump( gmstrftime($value, $timestamp) );
};

?>
--EXPECTF--
*** Testing gmstrftime() : usage variation ***

--Hour as decimal by 24-hour format--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(2) "%d"

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(2) "08"

--Hour as decimal by 12-hour format--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(2) "%d"

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(2) "08"

--Minute as decimal number--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(%d) "%d"

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(2) "08"

--AM/PM format for a time--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(2) "%s"

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(2) "AM"

--Second as decimal number--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(%d) "%d"

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(2) "08"
