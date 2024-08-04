--TEST--
GH-14537: shmop Windows 11 crashes the process
--EXTENSIONS--
shmop
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip only for Windows');
?>
--FILE--
<?php
$str = 'Hello World';

$shm_key = ftok(__FILE__, 'p');

$shm_id1 = shmop_open($shm_key, 'c', 0644, strlen($str));
shmop_delete($shm_id1);
var_dump($shm_id1);

$shm_id2 = shmop_open($shm_key, 'c', 0644, strlen($str) + 10);
var_dump($shm_id2);
?>
--EXPECTF--
object(Shmop)#1 (0) {
}

Warning: shmop_open(): Unable to attach or create shared memory segment "No error" in %s on line %d
bool(false)
