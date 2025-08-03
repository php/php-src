--TEST--
shmop_read errors
--EXTENSIONS--
shmop
--FILE--
<?php
$key = 9991;
$shm_id = shmop_open($key, 'n', 0o644, 1024);
try {
    $shmop = shmop_read($shm_id, -1, 100);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $shmop = shmop_read($shm_id, 1400, 100);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $shmop = shmop_read($shm_id, 0, -1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $shmop = shmop_read($shm_id, 0, 1400);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $shmop = shmop_read($shm_id, 1000, 25);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
shmop_delete($shm_id);
?>
--EXPECT--
ValueError: shmop_read(): Argument #2 ($offset) must be between 0 and the segment size of 1024
ValueError: shmop_read(): Argument #2 ($offset) must be between 0 and the segment size of 1024
ValueError: shmop_read(): Argument #3 ($size) must be greater than 0
ValueError: shmop_read(): Argument #3 ($size) is out of range
ValueError: shmop_read(): Argument #3 ($size) is out of range
