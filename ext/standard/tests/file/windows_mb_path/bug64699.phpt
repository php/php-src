--TEST--
Bug #64699 is_dir() is inaccurate result on Windows with japanese locale.
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip windows only test');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

/* This file is in UTF-8. */

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$old_cp = sapi_windows_cp_get();
sapi_windows_cp_set(65001);
echo "Active code page: ", sapi_windows_cp_get(), "\n";

$prefix = __DIR__ . DIRECTORY_SEPARATOR . "testBug64699" . DIRECTORY_SEPARATOR;

$dirs = array("a", "ソ", "ゾ", "şŞıİğĞ", "多国語", "表");

mkdir($prefix);
foreach ($dirs as $d) {
    mkdir($prefix . $d);
}

$dir = $prefix;
if ($dh = opendir($dir)) {
    while (($file = readdir($dh)) !== false) {
        $path = $dir . $file;
        $type = filetype($path);
        $type2= is_dir($path) ? 'dir' : 'file';
        $comp = $type == $type2 ? 'OK' : 'NG';
        echo "filetype()[".str_pad($type, 4)."] == is_dir()[".str_pad($type2, 4)."] -> $comp: {$file}\n";
    }
    closedir($dh);
}

foreach ($dirs as $d) {
    rmdir($prefix . $d);
}
rmdir($prefix);

sapi_windows_cp_set($old_cp);

?>
--EXPECT--
Active code page: 65001
filetype()[dir ] == is_dir()[dir ] -> OK: .
filetype()[dir ] == is_dir()[dir ] -> OK: ..
filetype()[dir ] == is_dir()[dir ] -> OK: a
filetype()[dir ] == is_dir()[dir ] -> OK: şŞıİğĞ
filetype()[dir ] == is_dir()[dir ] -> OK: ソ
filetype()[dir ] == is_dir()[dir ] -> OK: ゾ
filetype()[dir ] == is_dir()[dir ] -> OK: 多国語
filetype()[dir ] == is_dir()[dir ] -> OK: 表
