--TEST--
Bug #64699 is_dir() is inaccurate result on Windows with japanese locale.
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--FILE--
<?php

/* This file is in UTF-8. */

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$old_cp = get_active_cp();
set_active_cp(65001);

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

set_active_cp($old_cp);

?>
===DONE===
--EXPECTF--
Active code page: 65001
filetype()[dir ] == is_dir()[dir ] -> OK: .
filetype()[dir ] == is_dir()[dir ] -> OK: ..
filetype()[dir ] == is_dir()[dir ] -> OK: a
filetype()[dir ] == is_dir()[dir ] -> OK: şŞıİğĞ
filetype()[dir ] == is_dir()[dir ] -> OK: ソ
filetype()[dir ] == is_dir()[dir ] -> OK: ゾ
filetype()[dir ] == is_dir()[dir ] -> OK: 多国語
filetype()[dir ] == is_dir()[dir ] -> OK: 表
Active code page: %d
===DONE===
