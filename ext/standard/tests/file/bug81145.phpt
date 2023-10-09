--TEST--
Bug #81145 (copy() and stream_copy_to_stream() fail for +4GB files)
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (PHP_INT_SIZE !== 8) die("skip this test is for 64bit platforms only");
if (disk_free_space(__DIR__) < 0x220000000) die("skip insuffient disk space");
if (PHP_OS_FAMILY !== "Windows") {
    $src = __DIR__ . "/bug81145_src.bin";
    define('SIZE_4G', 0x100000000);
    exec("fallocate -l " . (SIZE_4G-0x100) . " " . escapeshellarg($src), $output, $status);
    if ($status !== 0) die("skip fallocate() not supported");
    @unlink(__DIR__ . "/bug81145_src.bin");
}
?>
--CONFLICTS--
all
--FILE--
<?php
$src = __DIR__ . "/bug81145_src.bin";
$dst = __DIR__ . "/bug81145_dst.bin";
define('SIZE_4G', 0x100000000);

//Create file and append random content at the 4GB boundary
if (PHP_OS_FAMILY !== "Windows") {
    exec("fallocate -l " . (SIZE_4G-0x100) . " " . escapeshellarg($src));
} else {
    exec("fsutil file createnew " . escapeshellarg($src) . " " . (SIZE_4G-0x100));
}
$fp = fopen($src, "ab");
fwrite($fp, random_bytes(0x200));
fclose($fp);
copy($src, $dst);
if (filesize($src) !== filesize($dst)) {
    die("Files have different sizes!");
}
$f1 = fopen($src,'rb') or die("src open failed");
$f2 = fopen($dst,'rb') or die("dst open failed");

//Seek to 4 GB boundary, as this is the location where the problem occurs
fseek($f1, SIZE_4G - 0x100, SEEK_SET);
fseek($f2, SIZE_4G - 0x100, SEEK_SET);
echo (fread($f1,0x200) === fread($f2,0x200) ? "Identical" : "Copy failed");
fclose($f1);
fclose($f2);
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/bug81145_src.bin");
@unlink(__DIR__ . "/bug81145_dst.bin");
?>
--EXPECT--
Identical
