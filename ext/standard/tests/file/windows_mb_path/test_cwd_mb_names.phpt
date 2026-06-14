--TEST--
Test chdir()/getcwd() with a dir for multibyte filenames
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip windows only test');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--CONFLICTS--
dir_mb
--FILE--
<?php


include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$prefix = create_data("dir_mb");
$dirw = $prefix . DIRECTORY_SEPARATOR . "テストマルチバイト・パス42";
touch($dirw . DIRECTORY_SEPARATOR . "dummy.txt");

$old_cp = sapi_windows_cp_get();
sapi_windows_cp_set(65001);
echo "Active code page: ", sapi_windows_cp_get(), "\n";

$oldcwd = getcwd();
var_dump(chdir($dirw));
var_dump(getcwd());

var_dump(file_exists("dummy.txt"));

sapi_windows_cp_set($old_cp);

chdir($oldcwd);
remove_data("dir_mb");

?>
--EXPECTF--
Active code page: 65001
bool(true)
string(%d) "%s\テストマルチバイト・パス42"
bool(true)
