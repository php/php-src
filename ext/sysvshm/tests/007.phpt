--TEST--
shm_remove() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
if (!function_exists('ftok')){ print 'skip'; }
?>
--FILE--
<?php

$key = ftok(__FILE__, 't');
$s = shm_attach($key, 1024);

var_dump(shm_remove());
var_dump(shm_remove(-1));
var_dump(shm_remove(0));
var_dump(shm_remove(""));

var_dump(shm_remove($s));

shm_detach($s);
var_dump(shm_remove($s));

echo "Done\n";
?>
--EXPECTF--
Warning: shm_remove() expects exactly 1 parameter, 0 given in %s007.php on line %d
NULL

Warning: shm_remove() expects parameter 1 to be resource, int given in %s007.php on line %d
NULL

Warning: shm_remove() expects parameter 1 to be resource, int given in %s007.php on line %d
NULL

Warning: shm_remove() expects parameter 1 to be resource, string given in %s007.php on line %d
NULL
bool(true)

Warning: shm_remove(): supplied resource is not a valid sysvshm resource in %s007.php on line %d
bool(false)
Done
