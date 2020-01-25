--TEST--
Cannot declare constants with function calls that contain objects
--FILE--
<?php
function make_object_array() {
    return [new stdClass()];
}
const OBJECT_VALUES = make_object_array();
?>
--EXPECTF--
Fatal error: Uncaught Error: Calls in constants may only evaluate to scalar values, arrays or resources in %s:5
Stack trace:
#0 {main}
  thrown in %s on line 5
