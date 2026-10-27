--TEST--
Failed retry mmap() in zend_mm_chunk_alloc_int() must not unmap an unrelated range
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux') die('skip Linux only');
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only');
if (getenv('USE_ZEND_ALLOC') === '0') die('skip Zend MM disabled');
$maps = @file_get_contents('/proc/self/maps');
if ($maps === false) die('skip /proc/self/maps not available');
if (!preg_match('/^([0-9a-f]+)-[0-9a-f]+ r-xp /m', $maps, $m) || hexdec($m[1]) > 0x40000000) {
    die('skip only for non-PIE builds');
}
?>
--INI--
memory_limit=-1
--FILE--
<?php
// A huge block whose size equals the start of the executable mapping. If the
// first mmap() is not 2 MiB aligned and the retry fails under RLIMIT_AS, the
// missing NULL check used to unmap [size, size + 2 MiB - 4 KiB), i.e. our text.
preg_match('/^([0-9a-f]+)-[0-9a-f]+ r-xp /m', file_get_contents('/proc/self/maps'), $m);
$size = hexdec($m[1]);
preg_match('/VmSize:\s+(\d+)/', file_get_contents('/proc/self/status'), $m);
// Room for the first mmap() of $size bytes, not for the aligned retry.
var_dump(posix_setrlimit(POSIX_RLIMIT_AS, $m[1] * 1024 + $size + 1024 * 1024, POSIX_RLIMIT_INFINITY));
$s = str_repeat('A', $size - 4096);
echo "allocation succeeded\n";
?>
--EXPECTREGEX--
bool\(true\)\s+(.*Out of memory|allocation succeeded)\s*
