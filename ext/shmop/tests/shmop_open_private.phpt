--TEST--
shmop_open with IPC_PRIVATE creates private SHM
--EXTENSIONS--
shmop
--FILE--
<?php
$write = 'test';

$shm1 = shmop_open(0, 'c', 0777, 1024);
shmop_write($shm1, $write, 0);

$shm2 = shmop_open(0, 'c', 0777, 1024);
$read = shmop_read($shm2, 0, 4);

var_dump(is_string($read) && $read !== $write);
?>
--EXPECT--
bool(true)
