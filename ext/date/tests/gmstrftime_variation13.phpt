--TEST--
Test gmstrftime() function : usage variation - Checking date related formats which was not supported on Windows before VC14.
--FILE--
<?php
echo "*** Testing gmstrftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$timestamp = gmmktime(8, 8, 8, 8, 8, 2008);
setlocale(LC_ALL, "C");
date_default_timezone_set("Asia/Calcutta");

//array of values to iterate over
$inputs = array(
      'Century number' => "%C",
      'Month Date Year' => "%D",
      'Year with century' => "%G",
      'Year without century' => "%g",
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

--Century number--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(2) "%d"

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(2) "20"

--Month Date Year--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(%d) "%d/%d/%d"

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(8) "08/08/08"

--Year with century--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(%d) "%d"

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(4) "2008"

--Year without century--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(2) "%d"

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(2) "08"
