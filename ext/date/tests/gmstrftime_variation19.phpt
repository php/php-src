--TEST--
Test gmstrftime() function : usage variation - Checking newline and tab formats which was not supported on Windows before VC14.
--FILE--
<?php
echo "*** Testing gmstrftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$timestamp = gmmktime(8, 8, 8, 8, 8, 2008);
setlocale(LC_ALL, "C");
date_default_timezone_set("Asia/Calcutta");

//array of values to iterate over
$inputs = array(
      'Newline character' => "%n",
      'Tab character' => "%t"
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

--Newline character--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(1) "
"

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(1) "
"

--Tab character--

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(1) "	"

Deprecated: Function gmstrftime() is deprecated in %s on line %d
string(1) "	"
