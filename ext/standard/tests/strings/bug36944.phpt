--TEST--
Bug #36944 (strncmp & strncasecmp do not return false on negative string length)
--FILE--
<?php

var_dump(strncmp("test ", "e", -1));
var_dump(strncmp("test ", "e", 10));
var_dump(strncmp("test ", "e", 0));

var_dump(strncasecmp("test ", "E", -1));
var_dump(strncasecmp("test ", "E", 10));
var_dump(strncasecmp("test ", "E", 0));

echo "Done\n";
?>
--EXPECTF--	
Warning: Length must be greater than or equal to 0 in %s on line %d
bool(false)
int(%d)
int(0)

Warning: Length must be greater than or equal to 0 in %s on line %d
bool(false)
int(%d)
int(0)
Done
