--TEST--
Basic ini_set error test
--INI--
max_execution_time=300
--FILE--
<?php
ini_set('report_ini_error', 1);
// No typo
var_dump(ini_set('max_execution_time', 300));
// Typo
var_dump(ini_set('max_exection_time', 300));
?>
===DONE===
--EXPECTF--
string(3) "300"

Warning: ini_set(): INI(max_exection_time) does not exist in %s on line %d
bool(false)
===DONE===

