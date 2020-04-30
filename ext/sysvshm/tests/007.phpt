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

var_dump(shm_remove($s));

shm_detach($s);
var_dump(shm_remove($s));

echo "Done\n";
?>
--EXPECTF--
bool(true)
bool(true)
Done
