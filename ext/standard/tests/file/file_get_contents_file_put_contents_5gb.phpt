--TEST--
Test file_put_contents() and file_get_contents() functions with 5GB string
--SKIPIF--
<?php
if (PHP_INT_SIZE < 5) {
    // 4=4gb, 5=549gb, 8=9exabytes
    die("skip PHP_INT_SIZE<5 will not fit test string in RAM");
}
if (getenv('SKIP_SLOW_TESTS')) {
    die('skip slow test');
}
include(__DIR__.'/../../../../tests/utils.php');
if(!has_enough_memory(10 * 1024 * 1024 * 1024)) { // 10GB
    die('skip Reason: Insufficient RAM (should be 10GB)');
}
$tmpfile = sys_get_temp_dir() . DIRECTORY_SEPARATOR . "file_get_contents_file_put_contents_5gb.bin";
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
$tmpfile = sys_get_temp_dir() . DIRECTORY_SEPARATOR . "file_get_contents_file_put_contents_5gb.bin";
$large_string_len = 5 * 1024 * 1024 * 1024;

$large_string = str_repeat('a', $large_string_len);
$result = file_put_contents($tmpfile, $large_string);
if ($result !== $large_string_len) {
    echo "Could only write $result bytes of $large_string_len bytes.";
    var_dump(error_get_last());
} else {
    echo "File written successfully." . PHP_EOL;
}
unset($large_string);

$result_large_string = file_get_contents($tmpfile);
if (strlen($result_large_string) !== $large_string_len) {
    echo "Could only read " . strlen($result_large_string) . " bytes of $large_string_len bytes.";
    var_dump(error_get_last());
} else {
    echo "File read successfully." . PHP_EOL;
}

clearstatcache(true, $tmpfile);
if (file_exists($tmpfile)) {
    unlink($tmpfile);
}
?>
--CLEAN--
<?php
@unlink(sys_get_temp_dir() . DIRECTORY_SEPARATOR . "file_get_contents_file_put_contents_5gb.bin");
?>
--EXPECT--
File written successfully.
File read successfully.
