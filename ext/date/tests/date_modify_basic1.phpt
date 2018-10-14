--TEST--
Test date_modify() function : basic functionality
--FILE--
<?php
/* Prototype  : DateTime date_modify  ( DateTime $object  , string $modify  )
 * Description: Alter the timestamp of a DateTime object by incrementing or decrementing in a format accepted by strtotime().
 * Source code: ext/date/php_date.c
 * Alias to functions: public DateTime DateTime::modify()
 */

 //Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing date_modify() : basic functionality ***\n";

// Create a date object to modify
$datetime = date_create("2009-01-31 14:28:41");

date_modify($datetime, "+1 day");
echo "After modification 1: " . date_format($datetime, "D, d M Y") . "\n";

date_modify($datetime, "+1 week 2 days 4 hours 2 seconds");
echo "After modification 2: " . date_format($datetime, "D, d M Y H:i:s") . "\n";

date_modify($datetime, "next Thursday");
echo "After modification 3: " . date_format($datetime, "D, d M Y") . "\n";

date_modify($datetime, "last Sunday");
echo "After modification 4: " . date_format($datetime, "D, d M Y") . "\n";

?>
===DONE===
--EXPECTF--
*** Testing date_modify() : basic functionality ***
After modification 1: Sun, 01 Feb 2009
After modification 2: Tue, 10 Feb 2009 18:28:43
After modification 3: Thu, 12 Feb 2009
After modification 4: Sun, 08 Feb 2009
===DONE===
