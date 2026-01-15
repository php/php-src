--TEST--
shmop Windows errno mapping
--EXTENSIONS--
shmop
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Windows") die("skip for Windows only");
?>
--FILE--
<?php
echo "EEXIST test:\n";
$key = 0xEEEEEEEE;
$shm1 = shmop_open($key, 'n', 0644, 512);
if ($shm1 !== false) {
    $shm2 = shmop_open($key, 'n', 0644, 512);
    shmop_delete($shm1);
}

echo "\nENOENT attach test:\n";
$shm = shmop_open(0x99999999, 'a', 0644, 0);

echo "\nENOENT write test:\n";
$shm = shmop_open(0x77777777, 'w', 0644, 0);

echo "\ndone\n";
?>
--EXPECTF--
EEXIST test:

Warning: shmop_open(): Unable to attach or create shared memory segment "File exists" in %s on line %d

ENOENT attach test:

Warning: shmop_open(): Unable to attach or create shared memory segment "No such file or directory" in %s on line %d

ENOENT write test:

Warning: shmop_open(): Unable to attach or create shared memory segment "No such file or directory" in %s on line %d

done
