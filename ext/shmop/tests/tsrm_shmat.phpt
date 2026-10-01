--TEST--
TSRM Windows shmop keeps the payload within its mapping
--EXTENSIONS--
shmop
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip only for Windows');
?>
--FILE--
<?php
$shm = shmop_open(0, 'c', 0644, 65472);
$payload = str_repeat('x', 65471) . 'z';
var_dump($shm !== false);
var_dump(shmop_size($shm));
var_dump(shmop_write($shm, $payload, 0));
var_dump(shmop_read($shm, 65471, 1));
var_dump(shmop_delete($shm));
?>
--EXPECT--
bool(true)
int(65472)
int(65472)
string(1) "z"
bool(true)
