--TEST--
shm_put_var() tests
--SKIPIF--
<?php if (!extension_loaded("sysvshm")) print "skip"; ?>
--FILE--
<?php

$key = ftok(__FILE__, 't');
$s = shm_attach($key, 1024);

var_dump(shm_put_var());
var_dump(shm_put_var(-1, -1, -1));
var_dump(shm_put_var(-1, 10, "qwerty"));
var_dump(shm_put_var($s, -1, "qwerty"));
var_dump(shm_put_var($s, 10, "qwerty"));
var_dump(shm_put_var($s, 10, "qwerty"));

$string = str_repeat("test", 512);
var_dump(shm_put_var($s, 11, $string));

shm_remove($s);

echo "Done\n";
?>
--EXPECTF--	
Warning: Wrong parameter count for shm_put_var() in %s on line %d
NULL

Warning: shm_put_var(): -1 is not a SysV shared memory index in %s on line %d
bool(false)

Warning: shm_put_var(): -1 is not a SysV shared memory index in %s on line %d
bool(false)
bool(true)
bool(true)
bool(true)

Warning: shm_put_var(): not enough shared memory left in %s on line %d
bool(false)
Done
