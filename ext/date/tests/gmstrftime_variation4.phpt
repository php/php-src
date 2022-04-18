--TEST--
Test gmstrftime() function : usage variation - Passing month related format strings to format argument.
--FILE--
<?php
echo "*** Testing gmstrftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$timestamp = gmmktime(8, 8, 8, 8, 8, 2008);

//array of values to iterate over
$inputs = array(
      'Abbreviated month name' => "%b",
      'Full month name' => "%B",
      'Month as decimal' => "%m",
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

--Abbreviated month name--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(%d) "%s"

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(3) "Aug"

--Full month name--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(%d) "%s"

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(6) "August"

--Month as decimal--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(%d) "%d"

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(2) "08"
