--TEST--
crypt(): *0 should return *1
--FILE--
<?php

var_dump(crypt('foo', '*0'));

?>
--EXPECT--
string(2) "*1"
