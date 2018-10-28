--TEST--
shm_put_var() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
if (!function_exists('ftok')){ print 'skip'; }
?>
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
Warning: shm_put_var() expects exactly 3 parameters, 0 given in %s004.php on line %d
NULL

Warning: shm_put_var() expects parameter 1 to be resource, int given in %s004.php on line %d
NULL

Warning: shm_put_var() expects parameter 1 to be resource, int given in %s004.php on line %d
NULL
bool(true)
bool(true)
bool(true)

Warning: shm_put_var(): not enough shared memory left in %s004.php on line 14
bool(false)
Done
