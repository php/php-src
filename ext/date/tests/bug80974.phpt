--TEST--
Bug #80974 (Wrong diff between 2 dates in different timezones)
--FILE--
<?php
date_default_timezone_set('UTC');
echo date_diff(
    new DateTime('2012-01-01 00:00 America/Toronto'),
    new DateTime('2012-01-01 00:00 America/Vancouver'),
)->format('%h'), "\n";
?>
--EXPECT--
3
