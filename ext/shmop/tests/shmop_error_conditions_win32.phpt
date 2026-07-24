--TEST--
shmop error conditions on Windows
--EXTENSIONS--
shmop
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Windows") die("skip for Windows only");
?>
--FILE--
<?php
echo "duplicate segment: ";
$key = 0xFEEDBEEF;
$shm1 = shmop_open($key, 'n', 0644, 1024);
$shm2 = shmop_open($key, 'n', 0644, 1024);
echo ($shm1 && !$shm2) ? "ok\n" : "failed\n";
shmop_delete($shm1);

echo "non-existent attach: ";
$shm = shmop_open(0xDEADC0DE, 'a', 0644, 0);
echo ($shm === false) ? "ok\n" : "failed\n";

echo "non-existent write: ";
$shm = shmop_open(0xBADF00D, 'w', 0644, 0);
echo ($shm === false) ? "ok\n" : "failed\n";

echo "zero size: ";
try {
    shmop_open(0x12340000, 'n', 0644, 0);
    echo "failed\n";
} catch (ValueError $e) {
    echo "ok\n";
}

echo "write/read test: ";
$key = 0xC0FFEE00;
$shm_c = shmop_open($key, 'c', 0644, 1024);
$shm_w = shmop_open($key, 'w', 0644, 0);
$shm_a = shmop_open($key, 'a', 0644, 0);
if ($shm_c && $shm_w && $shm_a) {
    $data = "test data";
    $written = shmop_write($shm_w, $data, 0);
    $read = shmop_read($shm_a, 0, strlen($data));
    echo ($read === $data) ? "ok\n" : "failed\n";
    shmop_delete($shm_c);
} else {
    echo "failed\n";
}

echo "multiple segments: ";
$segments = [];
for ($i = 0; $i < 3; $i++) {
    $shm = shmop_open(0xA0000000 + $i, 'c', 0644, 256);
    if ($shm) $segments[] = $shm;
}
echo (count($segments) === 3) ? "ok\n" : "failed\n";
foreach ($segments as $shm) {
    shmop_delete($shm);
}

echo "large segment: ";
$shm = shmop_open(0xBEEF0000, 'n', 0644, 1024 * 1024);
if ($shm) {
    echo (shmop_size($shm) >= 1024 * 1024) ? "ok\n" : "failed\n";
    shmop_delete($shm);
} else {
    echo "failed\n";
}
?>
--EXPECTF--
duplicate segment: 
Warning: shmop_open(): Unable to attach or create shared memory segment%s in %s on line %d
ok
non-existent attach: 
Warning: shmop_open(): Unable to attach or create shared memory segment%s in %s on line %d
ok
non-existent write: 
Warning: shmop_open(): Unable to attach or create shared memory segment%s in %s on line %d
ok
zero size: ok
write/read test: ok
multiple segments: ok
large segment: ok
