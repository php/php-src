--TEST--
sysvshm: shm_attach() must reject a segment whose header is out of bounds
--EXTENSIONS--
sysvshm
shmop
--SKIPIF--
<?php
if (PHP_INT_SIZE !== 8) die('skip 64-bit only');
?>
--FILE--
<?php
$key = ftok(__FILE__, 's');

$old = @shmop_open($key, 'w', 0, 0);
if ($old !== false) { shmop_delete($old); }

$seg = 4096;
$h = shmop_open($key, 'c', 0600, $seg);

shmop_write($h, pack('a8qqqq', 'PHP_SM', 40, 1 << 30, 0, $seg), 0);
shmop_write($h, pack('qqq', 1, 1 << 20, 32) . 's:1000000:"', 40);

$shm = shm_attach($key, $seg);
var_dump($shm);
if ($shm !== false) {
    var_dump(shm_get_var($shm, 1));
    shm_remove($shm);
}
echo "Done\n";
?>
--EXPECTF--
Warning: shm_attach(): Failed for key 0x%x: segment header is corrupted in %s on line %d
bool(false)
Done
--CLEAN--
<?php
$key = ftok(__FILE__, 's');
$h = @shmop_open($key, 'w', 0, 0);
if ($h !== false) { shmop_delete($h); }
?>
