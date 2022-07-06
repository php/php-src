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

var_dump(shm_put_var($s, -1, "qwerty"));
var_dump(shm_put_var($s, 10, "qwerty"));
var_dump(shm_put_var($s, 10, "qwerty"));

$string = str_repeat("test", 512);
var_dump(shm_put_var($s, 11, $string));

shm_remove($s);

echo "Done\n";
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)

Warning: shm_put_var(): Not enough shared memory left in %s004.php on line %d
bool(false)
Done
