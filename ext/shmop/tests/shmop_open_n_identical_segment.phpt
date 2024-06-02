--TEST--
Cannot create existing segment with n mode
--EXTENSIONS--
shmop
--FILE--
<?php
$key = 9980;
$shm_id = shmop_open($key, 'c', 0o644, 1024);
$shm_id2 = shmop_open($key, 'n', 0o644, 2048);
shmop_delete($shm_id);
?>
--EXPECTF--
Warning: shmop_open(): Unable to attach or create shared memory segment "File exists" in %s on line %d
