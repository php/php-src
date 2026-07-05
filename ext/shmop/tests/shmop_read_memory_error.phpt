--TEST--
shmop_read() throws a catchable MemoryError when the read size cannot fit in the memory limit
--EXTENSIONS--
shmop
--INI--
memory_limit=32M
opcache.enable_cli=0
--FILE--
<?php

$shm = shmop_open(ftok(__FILE__, 'n'), 'c', 0644, 64 * 1024 * 1024);

try {
    shmop_read($shm, 0, 0);
} catch (MemoryError $e) {
    echo $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(strlen(shmop_read($shm, 0, 4)));

shmop_delete($shm);

?>
--EXPECT--
MemoryError: The resulting string is too large to fit in the configured memory limit
int(4)
