--TEST--
Test clone on DateTime objects
--FILE--
<?php

//Set the default time zone
date_default_timezone_set('Europe/London');

echo "*** Testing clone on DateTime objects ***\n";

// Create a DateTime object..
$orig = new DateTime('2008-07-02 14:25:41');

// ..create a clone of it ..Clone
$clone = clone $orig;

// ..and modify original
$orig->setTime(22, 41, 50);

echo "Original: " . $orig->format("H:i:s") . "\n";
echo "Clone: " . $clone->format("H:i:s") . "\n";

?>
===DONE===
--EXPECT--
*** Testing clone on DateTime objects ***
Original: 22:41:50
Clone: 14:25:41
===DONE===
