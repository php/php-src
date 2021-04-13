--TEST--
Retrieve max_execution_wall_time ini value
--FILE--
<?php

var_dump(ini_get("max_execution_wall_time"));

ini_set("max_execution_wall_time", 1);

var_dump(ini_get("max_execution_wall_time"));

?>
--EXPECTF--
string(1) "0"
string(1) "1"
