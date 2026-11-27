--TEST--
Chunks with out of bounds sizes are ignored
--EXTENSIONS--
sysvshm
shmop
--FILE--
<?php
$key = 0x53484D33;

$s = shm_attach($key, 1024);
shm_put_var($s, 1, 'a');

$raw = shmop_open($key, 'w', 0, 0);
$next = 8 + 4 * PHP_INT_SIZE + 2 * PHP_INT_SIZE;
shmop_write($raw, pack(PHP_INT_SIZE === 8 ? 'q' : 'l', 0x10000), $next);

var_dump(shm_has_var($s, 1));
var_dump(shm_remove_var($s, 1));
var_dump(shm_put_var($s, 2, 'b'));

shm_remove($s);
?>
--EXPECTF--
bool(false)

Warning: shm_remove_var(): Variable key 1 doesn't exist in %s on line %d
bool(false)
bool(true)
--CLEAN--
<?php
$raw = @shmop_open(0x53484D33, 'w', 0, 0);
if ($raw) {
    shmop_delete($raw);
}
?>
