--TEST--
Bug #74852 property_exists returns true on unknown DateInterval property
--FILE--
<?php

$interval = new DateInterval('P2D');
var_dump(property_exists($interval,'abcde'));
var_dump(isset($interval->abcde));
var_dump($interval->abcde);

?>
--EXPECTF--
bool(false)
bool(false)

Notice: Undefined property: DateInterval::$abcde in %s on line %d
NULL
