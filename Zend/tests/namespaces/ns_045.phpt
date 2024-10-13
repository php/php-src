--TEST--
045: Name conflict and constants (php name in case if ns name exists)
--FILE--
<?php
namespace test\ns1;

const INI_ALL = 0;

var_dump(\INI_ALL);
?>
--EXPECT--
int(7)
