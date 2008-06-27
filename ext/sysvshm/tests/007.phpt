--TEST--
shm_remove() tests
--SKIPIF--
<?php if (!extension_loaded("sysvshm")) print "skip"; ?>
--FILE--
<?php

$key = ftok(__FILE__, 't');
$s = shm_attach($key, 1024);

var_dump(shm_remove());
var_dump(shm_remove(-1));
var_dump(shm_remove(0));
var_dump(shm_remove(""));

var_dump(shm_remove($s));
var_dump(shm_remove($s));

shm_detach($s);
var_dump(shm_remove($s));

echo "Done\n";
?>
--EXPECTF--	
Warning: shm_remove() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: shm_remove(): The parameter is not a valid shm_identifier in %s on line %d
bool(false)

Warning: shm_remove(): The parameter is not a valid shm_identifier in %s on line %d
bool(false)

Warning: shm_remove() expects parameter 1 to be long, string given in %s on line %d
NULL
bool(true)
bool(true)

Warning: shm_remove(): The parameter is not a valid shm_identifier in %s on line %d
bool(false)
Done
