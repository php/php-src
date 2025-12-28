--TEST--
047: Run-time name conflict and constants (php name)
--FILE--
<?php
namespace test\ns1;

const INI_ALL = 0;

var_dump(constant("INI_ALL"));
?>
--EXPECT--
int(7)
