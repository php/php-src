--TEST--
Shutdown crash when attached/removed same key segment multiple times
--EXTENSIONS--
sysvshm
--FILE--
<?php

/*$key = ftok(__FILE__, 't');
var_dump($key);*/
$key = 42;

var_dump($s = shm_attach($key, 1024));
shm_remove($s);
var_dump($s = shm_attach($key, 1024));
shm_remove($s);
?>
--EXPECT--
object(SysvSharedMemory)#1 (0) {
}
object(SysvSharedMemory)#2 (0) {
}
