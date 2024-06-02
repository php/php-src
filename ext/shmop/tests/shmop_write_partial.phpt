--TEST--
shmop_write with a partial write
--EXTENSIONS--
shmop
--FILE--
<?php
const SEGMENT_SIZE = 16;

$key = 9998;
$shm_id = shmop_open($key, 'n', 0o644, SEGMENT_SIZE);

var_dump(shmop_write($shm_id, str_repeat('a', SEGMENT_SIZE), /* offset */ 0));
var_dump(shmop_read($shm_id, 0, SEGMENT_SIZE));

try {
    var_dump(shmop_write($shm_id, str_repeat('b', 8), /* offset */ SEGMENT_SIZE-5));
    var_dump(shmop_read($shm_id, 0, SEGMENT_SIZE));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
shmop_delete($shm_id);
?>
--EXPECT--
int(16)
string(16) "aaaaaaaaaaaaaaaa"
int(5)
string(16) "aaaaaaaaaaabbbbb"
