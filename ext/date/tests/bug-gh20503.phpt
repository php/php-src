--TEST--
GH-20503 (Assertion failure with ext/date DateInterval property hash construction)
--SKIPIF--
<?php
if (ini_get('zend.max_allowed_stack_size') === false) {
    die('skip No stack limit support');
}
if (getenv('SKIP_ASAN')) {
    die('skip ASAN needs different stack limit setting due to more stack space usage');
}
?>
--INI--
zend.max_allowed_stack_size=512K
--FILE--
<?php
$obj = new DateInterval('P1W');
$obj->prop3 = $obj;
var_dump(json_encode($obj));
var_dump(json_last_error_msg());

$obj2 = new DateInterval('P1D');
$obj2->prop = [$obj2];
var_dump(json_encode($obj2));
var_dump(json_last_error_msg());
?>
--EXPECTF--
Deprecated: Creation of dynamic property DateInterval::$prop3 is deprecated in %s on line %d
bool(false)
string(28) "Maximum stack depth exceeded"

Deprecated: Creation of dynamic property DateInterval::$prop is deprecated in %s on line %d
bool(false)
string(18) "Recursion detected"
