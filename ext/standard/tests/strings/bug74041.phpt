--TEST--
Bug #74041: substr_count with length=0 broken
--FILE--
<?php

var_dump(substr_count("aaa", "a", 0, 0));
var_dump(substr_count("", "a", 0, 0));

?>
--EXPECT--
int(0)
int(0)
