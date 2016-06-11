--TEST--
Bug #70674 (ReflectionFunction::getClosure() leaks memory when used for internal functions)
--FILE--
<?php
var_dump(((new ReflectionFunction("strlen"))->getClosure())("hello"));
?>
--EXPECT--
int(5)
