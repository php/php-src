--TEST--
shm_attach() rejects an existing segment too small to hold its header
--EXTENSIONS--
sysvshm
shmop
--FILE--
<?php
$key = 0x53484D32;

$raw = shmop_open($key, 'n', 0600, 8);

var_dump(shm_attach($key, 1024));

shmop_delete($raw);
?>
--EXPECTF--
Warning: shm_attach(): Failed for key 0x%x: segment too small in %s on line %d
bool(false)
--CLEAN--
<?php
$raw = @shmop_open(0x53484D32, 'w', 0, 0);
if ($raw) {
    shmop_delete($raw);
}
?>
