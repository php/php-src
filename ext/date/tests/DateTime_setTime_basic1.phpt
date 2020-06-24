--TEST--
Test DateTime::setTime() function : basic functionality
--FILE--
<?php
 //Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing DateTime::setTime() : basic functionality ***\n";

// Create a DateTime object
$datetime = new DateTime("2009-01-31 15:14:10");

echo "Initial date: " . $datetime ->format(DATE_RFC2822) . "\n";

$datetime->setTime(17, 20);
echo "After modification1 " . $datetime ->format(DATE_RFC2822) . "\n";

$datetime->setTime(19, 05, 59);
echo "After modification2 " . $datetime ->format(DATE_RFC2822) . "\n";

$datetime->setTime(24, 10);
echo "After modification3 " . $datetime ->format(DATE_RFC2822) . "\n";

$datetime->setTime(47, 35, 47);
echo "After modification4 " . $datetime ->format(DATE_RFC2822) . "\n";

$datetime->setTime(54, 25);
echo "After modification5 " . $datetime ->format(DATE_RFC2822) . "\n";

?>
--EXPECT--
*** Testing DateTime::setTime() : basic functionality ***
Initial date: Sat, 31 Jan 2009 15:14:10 +0000
After modification1 Sat, 31 Jan 2009 17:20:00 +0000
After modification2 Sat, 31 Jan 2009 19:05:59 +0000
After modification3 Sun, 01 Feb 2009 00:10:00 +0000
After modification4 Mon, 02 Feb 2009 23:35:47 +0000
After modification5 Wed, 04 Feb 2009 06:25:00 +0000
