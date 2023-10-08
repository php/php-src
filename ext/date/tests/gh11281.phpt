--TEST--
GH-11281 (DateTimeZone::getName() does not include seconds in offset)
--FILE--
<?php
$tz = new DateTimeZone('+03:00');
echo $tz->getName(), "\n";
$tz = new DateTimeZone('+03:00:00');
echo $tz->getName(), "\n";
$tz = new DateTimeZone('-03:00:00');
echo $tz->getName(), "\n";
$tz = new DateTimeZone('+03:00:01');
echo $tz->getName(), "\n";
$tz = new DateTimeZone('-03:00:01');
echo $tz->getName(), "\n";
$tz = new DateTimeZone('+03:00:58');
echo $tz->getName(), "\n";
$tz = new DateTimeZone('-03:00:58');
echo $tz->getName(), "\n";
$tz = new DateTimeZone('+03:00:59');
echo $tz->getName(), "\n";
$tz = new DateTimeZone('-03:00:59');
echo $tz->getName(), "\n";
?>
--EXPECT--
+03:00
+03:00
-03:00
+03:00:01
-03:00:01
+03:00:58
-03:00:58
+03:00:59
-03:00:59
