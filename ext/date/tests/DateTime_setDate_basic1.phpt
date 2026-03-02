--TEST--
Test DateTime::setDate() function : basic functionality
--FILE--
<?php
 //Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing DateTime::setDate() : basic functionality ***\n";

$datetime = new DateTime("2009-01-30 19:34:10");

echo $datetime->format(DATE_RFC2822) . "\n";

$datetime->setDate(2008, 02, 01);

echo $datetime->format(DATE_RFC2822) . "\n";

?>
--EXPECT--
*** Testing DateTime::setDate() : basic functionality ***
Fri, 30 Jan 2009 19:34:10 +0000
Fri, 01 Feb 2008 19:34:10 +0000
