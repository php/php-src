--TEST--
Test gmdate() function : usage variation - Passing numeric representation of day formats.
--FILE--
<?php
echo "*** Testing gmdate() : usage variation ***\n";

// Initialise all required variables
date_default_timezone_set('UTC');
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

//array of values to iterate over
$inputs = array(

     'Day with leading zeros' => 'd',
     'Day without leading zeros' => 'j',
     'ISO representation' => 'N',
     'Numeric representation of day' => 'w',
     'Day of the year' => 'z'
);

// loop through each element of the array for timestamp

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( gmdate($value) );
      var_dump( gmdate($value, $timestamp) );
};

?>
--EXPECTF--
*** Testing gmdate() : usage variation ***

--Day with leading zeros--
string(%d) "%d"
string(2) "08"

--Day without leading zeros--
string(%d) "%d"
string(1) "8"

--ISO representation--
string(%d) "%d"
string(1) "5"

--Numeric representation of day--
string(%d) "%d"
string(1) "5"

--Day of the year--
string(%d) "%d"
string(3) "220"
