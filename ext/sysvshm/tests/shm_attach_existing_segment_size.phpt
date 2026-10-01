--TEST--
shm_attach() takes the size of an existing segment from the kernel, not from $size
--EXTENSIONS--
sysvshm
shmop
--FILE--
<?php
$key = 0x53484D31;

$raw = shmop_open($key, 'n', 0600, 4096);

$shm = shm_attach($key, 10 * 1024 * 1024);
var_dump($shm instanceof SysvSharedMemory);

var_dump(shm_put_var($shm, 1, str_repeat('A', 1024 * 1024)));

var_dump(shm_put_var($shm, 2, 'ok'));
var_dump(shm_get_var($shm, 2));

var_dump(shm_remove($shm));
?>
--EXPECTF--
bool(true)

Warning: shm_put_var(): Not enough shared memory left in %s on line %d
bool(false)
bool(true)
string(2) "ok"
bool(true)
--CLEAN--
<?php
$raw = @shmop_open(0x53484D31, 'w', 0, 0);
if ($raw) {
    shmop_delete($raw);
}
?>
