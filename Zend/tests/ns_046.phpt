--TEST--
046: Run-time name conflict and constants (ns name)
--FILE--
<?php
namespace test\ns1;

const INI_ALL = 0;

var_dump(constant("test\\ns1\\INI_ALL"));
--EXPECT--
int(0)
