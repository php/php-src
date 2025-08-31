--TEST--
pcntl_exec() 3
--EXTENSIONS--
pcntl
--FILE--
<?php
$file = tempnam(sys_get_temp_dir(),"php");
var_dump(pcntl_exec($file, array("foo","bar"), array("foo" => "bar")));
unlink($file);
?>
--EXPECTF--
Warning: pcntl_exec(): Error has occurred: (errno %d) %s
bool(false)
