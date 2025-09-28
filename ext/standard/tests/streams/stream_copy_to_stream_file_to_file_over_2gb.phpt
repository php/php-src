--TEST--
stream_copy_to_stream() copies files larger than 2GB in full
--SKIPIF--
<?php
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (PHP_INT_SIZE < 8) die('skip 64-bit only');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
$dir = sys_get_temp_dir();
if (disk_free_space($dir) < 4 * 1024 * 1024 * 1024) {
    die('skip Reason: Insufficient disk space (less than 4GB)');
}
?>
--FILE--
<?php
$size = 3 * 1024 * 1024 * 1024; // exceeds the ~2GB per-call kernel copy limit
$src = sys_get_temp_dir() . DIRECTORY_SEPARATOR . "stream_copy_over_2gb_src.bin";
$dst = sys_get_temp_dir() . DIRECTORY_SEPARATOR . "stream_copy_over_2gb_dst.bin";

// Create a sparse 3GB source so the copy loop runs without using 3GB of data.
$fh = fopen($src, "wb");
fseek($fh, $size - 1);
fwrite($fh, "\0");
fclose($fh);

$in = fopen($src, "rb");
$out = fopen($dst, "wb");
$copied = stream_copy_to_stream($in, $out);
fclose($in);
fclose($out);

var_dump($copied === $size);
var_dump(filesize($dst) === $size);

unlink($src);
unlink($dst);
?>
--EXPECT--
bool(true)
bool(true)
--CLEAN--
<?php
@unlink(sys_get_temp_dir() . DIRECTORY_SEPARATOR . "stream_copy_over_2gb_src.bin");
@unlink(sys_get_temp_dir() . DIRECTORY_SEPARATOR . "stream_copy_over_2gb_dst.bin");
?>
