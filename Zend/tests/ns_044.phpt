--TEST--
044: Name conflict and constants (php name)
--FILE--
<?php
namespace test\ns1;

var_dump(INI_ALL);
--EXPECT--
int(7)
