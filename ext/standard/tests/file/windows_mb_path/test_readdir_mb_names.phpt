--TEST--
Test readdir() with a dir for multibyte filenames
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_no_required_exts();
skip_if_not_win();

?>
--FILE--
<?php

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$dirw = dirname(__FILE__) . DIRECTORY_SEPARATOR . "data" . DIRECTORY_SEPARATOR;

$old_cp = get_active_cp();
set_active_cp(65001);
setlocale(LC_ALL, "UTF-8");
if (is_dir($dirw)) {
    if ($dh = opendir($dirw)) {
        while (($file = readdir($dh)) !== false) {
            echo "filename: $file : filetype: " . filetype($dirw . $file) . "\n";
        }
        closedir($dh);
    }
} else {
	echo "is_dir failed\n";
}
set_active_cp($old_cp);

?>
===DONE===
--EXPECTF--
Active code page: 65001
filename: . : filetype: dir
filename: .. : filetype: dir
filename: Röd_Statistics.txt : filetype: file
filename: tschüß : filetype: file
filename: tschüß3 : filetype: dir
filename: Voláçao : filetype: file
filename: Voláçao3 : filetype: dir
filename: českýtestování.inc : filetype: file
filename: š.txt : filetype: file
filename: привет : filetype: file
filename: привет3 : filetype: dir
filename: テストマルチバイト・パス : filetype: file
filename: テストマルチバイト・パス42 : filetype: dir
filename: 測試多字節路徑 : filetype: file
filename: 測試多字節路徑5 : filetype: dir
Active code page: %d
===DONE===
