--TEST--
shmop extension error messages
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--SKIPIF--
<?php
    if (!extension_loaded("shmop")) {
        die("skip shmop() extension not available");
    }
?>
--FILE--
<?php

echo PHP_EOL, '## shmop_open function tests ##', PHP_EOL;

// Invalid flag when the flags length != 1
try {
    shmop_open(1338, '', 0644, 1024);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    shmop_open(1338, 'b', 0644, 1024);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

// Warning outputs: Unable to attach or create shared memory segment
var_dump(shmop_open(null, 'a', 0644, 1024));

// Shared memory segment size must be greater than zero
try {
    shmop_open(null, 'a', 0644, 1024);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

//Shared memory segment size must be greater than zero
try {
    shmop_open(1338, "c", 0666, 0);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo PHP_EOL, '## shmop_read function tests ##', PHP_EOL;
// Start is out of range
$shm_id = shmop_open(1338, 'n', 0600, 1024);
try {
    shmop_read($shm_id, -10, 0);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
shmop_delete($shm_id);

// Count is out of range
$shm_id = shmop_open(1339, 'n', 0600, 1024);
try {
    shmop_read($shm_id, 0, -10);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
shmop_delete($shm_id);

echo PHP_EOL, '## shmop_write function tests ##', PHP_EOL;
// Offset out of range
$shm_id = shmop_open(1340, 'n', 0600, 1024);
try {
    shmop_write($shm_id, 'text to try write', -10);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
shmop_delete($shm_id);
?>
--EXPECTF--
## shmop_open function tests ##
shmop_open(): Argument #2 ($mode) must be a valid access mode
shmop_open(): Argument #2 ($mode) must be a valid access mode

Warning: shmop_open(): Unable to attach or create shared memory segment "%s" in %s on line %d
bool(false)

Warning: shmop_open(): Unable to attach or create shared memory segment "%s" in %s on line %d
shmop_open(): Argument #4 ($size) must be greater than 0 for the "c" and "n" access modes

## shmop_read function tests ##
shmop_read(): Argument #2 ($offset) must be between 0 and the segment size
shmop_read(): Argument #3 ($size) is out of range

## shmop_write function tests ##
shmop_write(): Argument #3 ($offset) is out of range
