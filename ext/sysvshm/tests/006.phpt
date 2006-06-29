--TEST--
shm_remove_var() tests
--SKIPIF--
<?php if (!extension_loaded("sysvshm")) print "skip"; ?>
--FILE--
<?php

$key = ftok(__FILE__, 't');
$s = shm_attach($key, 1024);

shm_put_var($s, 1, "test string");

var_dump(shm_remove_var());
var_dump(shm_remove_var(-1, -1));
var_dump(shm_remove_var($s, -10));

var_dump(shm_get_var($s, 1));

var_dump(shm_remove_var($s, 1));
var_dump(shm_get_var($s, 1));

var_dump(shm_remove_var($s, 1));
var_dump(shm_get_var($s, 1));

shm_remove($s);
echo "Done\n";
?>
--EXPECTF--	
Warning: Wrong parameter count for shm_remove_var() in %s on line %d
NULL

Warning: shm_remove_var(): -1 is not a SysV shared memory index in %s on line %d
bool(false)

Warning: shm_remove_var(): variable key -10 doesn't exist in %s on line %d
bool(false)
string(11) "test string"
bool(true)

Warning: shm_get_var(): variable key 1 doesn't exist in %s on line %d
bool(false)

Warning: shm_remove_var(): variable key 1 doesn't exist in %s on line %d
bool(false)

Warning: shm_get_var(): variable key 1 doesn't exist in %s on line %d
bool(false)
Done
