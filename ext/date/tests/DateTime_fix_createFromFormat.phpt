--TEST--
Test fix for DateTime when date have textual day with dot or other special char at end
--FILE--
<?php

//Set the default time zone
date_default_timezone_set('Europe/London');

echo "*** Testing clone on DateTime objects ***\n";

// Create a DateTime object..
$orig = new DateTime('2012-11-29 17:00:00');

// String to parse
$string = "Thu., Nov. 29, 2012 5:00PM";

// Create a DateTime object from format
$fromFormat = DateTime::createFromFormat( "D., M# j, Y g:iA", $string );

echo "Format method: " . $orig->format("D., M. j, Y g:iA") . "\n";
echo "createFromFormat method: " . $fromFormat->format("D., M. j, Y g:iA") . "\n";

?>
===DONE===
--EXPECTF--
*** Testing clone on DateTime objects ***
Format method: Thu., Nov. 29, 2012 5:00PM
createFromFormat method: Thu., Nov. 29, 2012 5:00PM
===DONE===
