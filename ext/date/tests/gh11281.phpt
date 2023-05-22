--TEST--
Bug GH-11281 (DateTimeZone::getName() does not include seconds)
--FILE--
<?php
$tz = new DateTimeZone('+03:00:01');
echo $tz->getName();
?>
--EXPECT--
+03:00:01
