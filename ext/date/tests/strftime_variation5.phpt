--TEST--
Test strftime() function : usage variation - Passing date related format strings to format argument.
--FILE--
<?php
echo "*** Testing strftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
setlocale(LC_ALL, "en_US");
date_default_timezone_set("Asia/Calcutta");
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

//array of values to iterate over
$inputs = array(
      'Year as decimal number without a century' => "%y",
      'Year as decimal number including the century' => "%Y",
      'Time zone offset' => "%Z",
      'Time zone offset' => "%z",
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

--Year as decimal number without a century--

Deprecated: Function strftime() is deprecated in %s on line %d
string(%d) "%d"

Deprecated: Function strftime() is deprecated in %s on line %d
string(2) "08"

--Year as decimal number including the century--

Deprecated: Function strftime() is deprecated in %s on line %d
string(%d) "%d"

Deprecated: Function strftime() is deprecated in %s on line %d
string(4) "2008"

--Time zone offset--

Deprecated: Function strftime() is deprecated in %s on line %d
string(%d) "%s"

Deprecated: Function strftime() is deprecated in %s on line %d
string(%d) "%s"
