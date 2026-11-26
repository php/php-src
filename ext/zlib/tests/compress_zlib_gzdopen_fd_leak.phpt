--TEST--
compress.zlib:// does not leak a file descriptor when gzdopen() rejects the mode
--EXTENSIONS--
zlib
--SKIPIF--
<?php
if (!is_dir('/proc/self/fd')) die('skip requires /proc/self/fd');
?>
--FILE--
<?php
$file = __DIR__ . '/compress_zlib_gzdopen_fd_leak.tmp';
$countFds = fn() => count(scandir('/proc/self/fd'));

$before = $countFds();
for ($i = 0; $i < 10; $i++) {
    var_dump(@touch('compress.zlib://' . $file));
}
var_dump($countFds() - $before);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/compress_zlib_gzdopen_fd_leak.tmp');
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
int(0)
