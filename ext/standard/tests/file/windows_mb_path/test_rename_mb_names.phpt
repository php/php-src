--TEST--
Test rename() with a dir for multibyte filenames
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip windows only test');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--CONFLICTS--
file2_mb
--FILE--
<?php

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$prefix = create_data("file2_mb");

$fw_orig = $prefix . DIRECTORY_SEPARATOR . "Ελλάδα.txt";
$fw_copied = $prefix . DIRECTORY_SEPARATOR . "Ελλάδα_copy.txt";
$fw_renamed = $prefix . DIRECTORY_SEPARATOR . "測試多字節路徑17.txt";

$old_cp = sapi_windows_cp_get();
sapi_windows_cp_set(65001);
echo "Active code page: ", sapi_windows_cp_get(), "\n";

var_dump(copy($fw_orig, $fw_copied));
var_dump(get_basename($fw_copied, false));
var_dump(file_exists($fw_copied));

var_dump(rename($fw_copied, $fw_renamed));
var_dump(get_basename($fw_renamed, false));
var_dump(file_exists($fw_renamed));

var_dump(unlink($fw_renamed));

sapi_windows_cp_set($old_cp);

remove_data("file2_mb");

?>
--EXPECT--
Active code page: 65001
bool(true)
string(21) "Ελλάδα_copy.txt"
bool(true)
bool(true)
string(27) "測試多字節路徑17.txt"
bool(true)
bool(true)
