--TEST--
Test file_put_contents() function with 5GB string
--SKIPIF--
<?php
if (PHP_INT_SIZE < 5) {
    // 4=4gb, 5=549gb, 8=9exabytes
    skip("skip PHP_INT_SIZE<5 will not fit test string in RAM");
}
if (getenv('SKIP_SLOW_TESTS')) {
    die('skip slow test');
}
function get_system_memory(): int|float|false
{
    if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') {
        // Windows-based memory check
        @exec('wmic OS get FreePhysicalMemory', $output);
        if (isset($output[1])) {
            return ((int)trim($output[1])) * 1024;
        }
    } else {
        // Unix/Linux-based memory check
        $memInfo = @file_get_contents("/proc/meminfo");
        if ($memInfo) {
            preg_match('/MemFree:\s+(\d+) kB/', $memInfo, $matches);
            return $matches[1] * 1024; // Convert to bytes
        }
    }
    return false;
}
if (get_system_memory() < 10 * 1024 * 1024 * 1024) {
    die('skip Reason: Insufficient RAM (less than 10GB)');
}
$tmpfile = sys_get_temp_dir() . DIRECTORY_SEPARATOR . "test_file_put_contents_5gb.bin";
$tmpfileh = fopen($tmpfile, "wb");
if ($tmpfileh === false) {
    die('skip Reason: Unable to create temporary file');
}
fclose($tmpfileh);
unlink($tmpfile);
if (disk_free_space(dirname($tmpfile)) < 10 * 1024 * 1024 * 1024) {
    die('skip Reason: Insufficient disk space (less than 10GB)');
}
?>
--INI--
memory_limit=6G
--FILE--
<?php
$tmpfile = sys_get_temp_dir() . DIRECTORY_SEPARATOR . "test_file_put_contents_5gb.bin";
$large_string = str_repeat('a', 5 * 1024 * 1024 * 1024);
$result = file_put_contents($tmpfile, $large_string);
if ($result !== strlen($large_string)) {
    echo "Could only write $result bytes of " . strlen($large_string) . " bytes.";
    var_dump(error_get_last());
} else {
    echo "File written successfully.";
}
clearstatcache(true, $tmpfile);
if (file_exists($tmpfile)) {
    unlink($tmpfile);
}
?>
--CLEAN--
<?php
@unlink(sys_get_temp_dir() . DIRECTORY_SEPARATOR . "test_file_put_contents_5gb.bin");
?>
--EXPECT--
File written successfully.
