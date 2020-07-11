--TEST--
Test DateTime::getOffset() function : basic functionality
--FILE--
<?php
//Set the default time zone
date_default_timezone_set('Europe/London');

echo "*** Testing DateTime::getOffset() : basic functionality ***\n";

$winter = new DateTime('2008-12-25 14:25:41');
$summer = new DateTime('2008-07-02 14:25:41');

echo "Winter offset: " . $winter->getOffset() / 3600 . " hours\n";
echo "Summer offset: " . $summer->getOffset() / 3600 . " hours\n";

?>
--EXPECT--
*** Testing DateTime::getOffset() : basic functionality ***
Winter offset: 0 hours
Summer offset: 1 hours
