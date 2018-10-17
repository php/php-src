--TEST--
Test DateTime::modify() function : basic functionality
--FILE--
<?php
/* Prototype  : public DateTime DateTime::modify  ( string $modify  )
 * Description: Alter the timestamp of a DateTime object by incrementing or decrementing in a format accepted by strtotime().
 * Source code: ext/date/php_date.c
 * Alias to functions: public date_modify()
 */

 //Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing DateTime::modify() : basic functionality ***\n";

// Create a date object to modify
$datetime = new DateTime("2009-01-31 14:28:41");

$datetime->modify("+1 day");
echo "After modification 1: " . $datetime->format("D, d M Y") . "\n";

$datetime->modify("+1 week 2 days 4 hours 2 seconds");
echo "After modification 2: " . $datetime->format("D, d M Y H:i:s") . "\n";

$datetime->modify("next Thursday");
echo "After modification 3: " . $datetime->format("D, d M Y") . "\n";

$datetime->modify("last Sunday");
echo "After modification 4: " . $datetime->format("D, d M Y") . "\n";

?>
===DONE===
--EXPECT--
*** Testing DateTime::modify() : basic functionality ***
After modification 1: Sun, 01 Feb 2009
After modification 2: Tue, 10 Feb 2009 18:28:43
After modification 3: Thu, 12 Feb 2009
After modification 4: Sun, 08 Feb 2009
===DONE===
