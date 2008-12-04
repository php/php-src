--TEST--
043: Name conflict and constants (ns name)
--FILE--
<?php
namespace test\ns1;

const INI_ALL = 0;

var_dump(INI_ALL);
--EXPECT--
int(0)
