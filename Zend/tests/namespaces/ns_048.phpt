--TEST--
048: __NAMESPACE__ constant and runtime names (ns name)
--FILE--
<?php
namespace test\ns1;

const FOO = 0;

var_dump(constant(__NAMESPACE__ . "\\FOO"));
?>
--EXPECT--
int(0)
