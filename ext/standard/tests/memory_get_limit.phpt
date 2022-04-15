--TEST--
Test memory_get_limit
--INI--
memory_limit=10M
--FILE--
<?php

var_dump(memory_get_limit());

ini_set('memory_limit', '20M');
var_dump(memory_get_limit());

ini_set('memory_limit', '1G');
var_dump(memory_get_limit());

ini_set('memory_limit', -1);
var_dump(memory_get_limit());

ini_set('memory_limit', 0);
var_dump(memory_get_limit());

?>
--EXPECTF--
int(10485760)
int(20971520)
int(1073741824)
int(-1)

Warning: Failed to set memory limit to 0 bytes (Current memory usage is %d bytes) in %s on line %d
int(-1)
