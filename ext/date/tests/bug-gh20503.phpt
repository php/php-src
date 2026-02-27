--TEST--
GH-20503 (Assertion failure with ext/date DateInterval property hash construction)
--FILE--
<?php
$obj = new DateInterval('P1W');
$obj->prop3 = $obj;

// Array cast triggers get_properties_for with ARRAY_CAST purpose
// This would previously cause an assertion failure when modifying
// a HashTable with refcount > 1
$props = (array) $obj;
var_dump(count($props));
var_dump(isset($props['prop3']));
var_dump($props['y']);
?>
--EXPECTF--
Deprecated: Creation of dynamic property DateInterval::$prop3 is deprecated in %s on line %d
int(11)
bool(true)
int(0)
