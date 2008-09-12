--TEST--
pcntl_exec() 3
--FILE--
<?php
var_dump(pcntl_exec());
$file = tempnam(sys_get_temp_dir(),"php");
var_dump(pcntl_exec($file, array("foo","bar"), array("foo" => "bar")));
unlink($file);
?>
--EXPECTF--
Warning: pcntl_exec() expects at least 1 parameter, 0 given %s
NULL

Warning: pcntl_exec(): Error has occured: (errno %d) %s
bool(false)
