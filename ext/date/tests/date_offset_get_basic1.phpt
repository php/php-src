--TEST--
Test date_offset_get() function : basic functionality
--FILE--
<?php
/* Prototype  : int date_offset_get  ( DateTime $object  )
 * Description: Returns the daylight saving time offset
 * Source code: ext/date/php_date.c
 * Alias to functions:  DateTime::getOffset
 */

//Set the default time zone
date_default_timezone_set('Europe/London');

echo "*** Testing date_offset_get() : basic functionality ***\n";

$winter = date_create('2008-12-25 14:25:41');
$summer = date_create('2008-07-02 14:25:41');

echo "Winter offset: " . date_offset_get($winter) / 3600 . " hours\n";
echo "Summer offset: " . date_offset_get($summer) / 3600 . " hours\n";

?>
===DONE===
--EXPECT--
*** Testing date_offset_get() : basic functionality ***
Winter offset: 0 hours
Summer offset: 1 hours
===DONE===
