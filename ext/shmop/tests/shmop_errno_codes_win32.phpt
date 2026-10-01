--TEST--
shmop errno codes on Windows
--EXTENSIONS--
shmop
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Windows") die("skip for Windows only");
?>
--FILE--
<?php
$key = 0xABCDEF01;
$shm1 = shmop_open($key, 'n', 0644, 1024);
if ($shm1 !== false) {
    $shm2 = shmop_open($key, 'n', 0644, 1024);
    shmop_delete($shm1);
}
echo "done\n";
?>
--EXPECTF--
Warning: shmop_open(): Unable to attach or create shared memory segment "File exists" in %s on line %d
done
