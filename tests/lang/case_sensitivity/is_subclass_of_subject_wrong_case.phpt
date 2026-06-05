--TEST--
is_subclass_of() and is_a() with wrong-case subject string fails with wrong case
--FILE--
<?php
class Base {}
class Child extends Base {}

var_dump(is_subclass_of("CHILD", "Base"));
var_dump(is_subclass_of("child", "Base"));
var_dump(is_subclass_of("Child", "Base"));

// is_a() with allow_string=true (third arg)
var_dump(is_a("CHILD", "Base", true));
?>
--EXPECTF--
bool(false)
bool(false)
bool(true)
bool(false)
