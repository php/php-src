--TEST--
shmop_open errors
--EXTENSIONS--
shmop
--FILE--
<?php
$key = 9990;
try {
    $shmop = shmop_open($key, '', 0, 0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $shmop = shmop_open($key, 'wrong_mode', 0, 0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $shmop = shmop_open($key, 'q', 0, 0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $shmop = shmop_open($key, 'r', -1, 0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $shmop = shmop_open($key, 'r', 0, -1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $shmop = shmop_open($key, 'c', 0, 0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ValueError: shmop_open(): Argument #2 ($mode) must be a valid access mode, which is one of "a", "c", "n", or "w"
ValueError: shmop_open(): Argument #2 ($mode) must be a valid access mode, which is one of "a", "c", "n", or "w"
ValueError: shmop_open(): Argument #2 ($mode) must be a valid access mode, which is one of "a", "c", "n", or "w"
ValueError: shmop_open(): Argument #3 ($permissions) must be greater or equal than 0
ValueError: shmop_open(): Argument #4 ($size) must be greater or equal than 0
ValueError: shmop_open(): Argument #4 ($size) must be greater than 0 for the "c" and "n" access modes
