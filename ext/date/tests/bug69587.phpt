--TEST--
Bug #69587 (DateInterval properties and isset)
--FILE--
<?php
$datetime1 = new DateTime('2009-10-11');
$datetime2 = new DateTime('2009-10-13');
$interval = $datetime1->diff($datetime2);

var_dump(property_exists($interval, 'm'), isset($interval->m), empty($interval->m), empty($interval->d));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
