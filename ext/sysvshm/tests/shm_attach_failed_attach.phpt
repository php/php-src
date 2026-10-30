--TEST--
shm_attach() removes the segment it created when shmat() fails
--EXTENSIONS--
sysvshm
posix
--SKIPIF--
<?php
if (posix_geteuid() === 0) die('skip cannot run as root');
?>
--FILE--
<?php
$key = ftok(__FILE__, 't');

var_dump(shm_attach($key, 1024, 0));

$segment = shm_attach($key, 1024, 0600);

if (!$segment instanceof SysvSharedMemory) {
    die("the key is still held by the segment of the failed attach\n");
}

try {
    var_dump(shm_put_var($segment, 1, 'value'));
    var_dump(shm_get_var($segment, 1));
} finally {
    var_dump(shm_remove($segment));
}
?>
--EXPECTF--
Warning: shm_attach(): Failed for key 0x%x: %s in %s on line %d
bool(false)
bool(true)
string(5) "value"
bool(true)
