--TEST--
Bug #81216: Nullsafe operator leaks dynamic property name
--FILE--
<?php
$str = "foo";
null?->{$str . "bar"};
?>
DONE
--EXPECT--
DONE
