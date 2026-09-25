--TEST--
shmop errno handling tests
--EXTENSIONS--
shmop
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Windows") die("skip for Windows only");
?>
--FILE--
<?php
$key = 0x12345678;
$size = 1024;

echo "create segment: ";
$shm1 = shmop_open($key, 'n', 0644, $size);
if ($shm1 === false) {
    die("failed\n");
}
echo "ok\n";

echo "duplicate with IPC_EXCL: ";
$shm2 = shmop_open($key, 'n', 0644, $size);
if ($shm2 === false) {
    echo "ok\n";
} else {
    echo "failed\n";
    shmop_delete($shm2);
}
shmop_delete($shm1);

echo "attach non-existent: ";
$shm = shmop_open(0x99999999, 'a', 0644, 0);
echo ($shm === false) ? "ok\n" : "failed\n";

echo "write mode non-existent: ";
$shm = shmop_open(0x88888888, 'w', 0644, 0);
echo ($shm === false) ? "ok\n" : "failed\n";

echo "create/attach/write sequence: ";
$key = 0x45678901;
$shm_c = shmop_open($key, 'c', 0644, 512);
$shm_a = shmop_open($key, 'a', 0644, 0);
$shm_w = shmop_open($key, 'w', 0644, 0);
if ($shm_c && $shm_a && $shm_w) {
    $data = "test";
    $written = shmop_write($shm_w, $data, 0);
    $read = shmop_read($shm_a, 0, strlen($data));
    echo ($read === $data) ? "ok\n" : "failed\n";
    shmop_delete($shm_c);
} else {
    echo "failed\n";
}
?>
--EXPECTF--
create segment: ok
duplicate with IPC_EXCL: 
Warning: shmop_open(): Unable to attach or create shared memory segment%s in %s on line %d
ok
attach non-existent: 
Warning: shmop_open(): Unable to attach or create shared memory segment%s in %s on line %d
ok
write mode non-existent: 
Warning: shmop_open(): Unable to attach or create shared memory segment%s in %s on line %d
ok
create/attach/write sequence: ok
