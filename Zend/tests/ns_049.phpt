--TEST--
049: __NAMESPACE__ constant and runtime names (php name)
--FILE--
<?php
const FOO = 0;

var_dump(constant(__NAMESPACE__ . "\\FOO"));
?>
--EXPECT--
int(0)
