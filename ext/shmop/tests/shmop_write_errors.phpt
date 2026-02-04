--TEST--
shmop_write errors
--EXTENSIONS--
shmop
--FILE--
<?php
$key = 9992;
$shm_id = shmop_open($key, 'n', 0o644, 16);
try {
    var_dump(shmop_write($shm_id, 'text to try write', -10));
    var_dump(shmop_read($shm_id, 0, 16));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(shmop_write($shm_id, 'text to try write', 20));
    var_dump(shmop_read($shm_id, 0, 16));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
shmop_delete($shm_id);
?>
--EXPECT--
ValueError: shmop_write(): Argument #3 ($offset) must be between 0 and the segment size of 16
ValueError: shmop_write(): Argument #3 ($offset) must be between 0 and the segment size of 16
