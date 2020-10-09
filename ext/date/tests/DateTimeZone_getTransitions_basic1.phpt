--TEST--
Test DateTimeZone::getTransitions() function : basic functionality
--FILE--
<?php
echo "*** Testing DateTimeZone::getTransitions() : basic functionality ***\n";

//Set the default time zone
date_default_timezone_set("Europe/London");

// Create a DateTimeZone object
$tz = new DateTimeZone("Europe/London");

$tran = $tz->getTransitions(-306972000, -37241999);

if (!is_array($tran)) {
    echo "TEST FAILED: Expected an array\n";
}

echo "\n-- Total number of transitions: " . count($tran). " --\n";

echo "\n-- Format a sample entry for Spring 1963 --\n";
var_dump( $tran[6] );

?>
--EXPECT--
*** Testing DateTimeZone::getTransitions() : basic functionality ***

-- Total number of transitions: 18 --

-- Format a sample entry for Spring 1963 --
array(5) {
  ["ts"]=>
  int(-213228000)
  ["time"]=>
  string(24) "1963-03-31T02:00:00+0000"
  ["offset"]=>
  int(3600)
  ["isdst"]=>
  bool(true)
  ["abbr"]=>
  string(3) "BST"
}
