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
Fatal error: Constant expression uses function make_object_array() which is not in get_const_expr_functions() in %s on line 5