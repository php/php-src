--TEST--
Test readdir() with a dir for multibyte filenames
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip windows only test');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--CONFLICTS--
mb_names
--FILE--
<?php

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$prefix = create_data("mb_names");
$content = "";
create_verify_file($prefix, "českýtestování.inc", $content);
create_verify_file($prefix, "Röd_Statistics.txt", $content);
create_verify_file($prefix, "š.txt", "");
create_verify_file($prefix, "tschüß", $content);
create_verify_file($prefix, "Voláçao", "hola");
create_verify_file($prefix, "Ελλάδα.txt", "");
create_verify_file($prefix, "привет", "opened an utf8 filename for reading");
create_verify_file($prefix, "テストマルチバイト・パス", $content);
create_verify_file($prefix, "測試多字節路徑", $content);
create_verify_file($prefix, "żółć.txt", $content);
create_verify_dir($prefix, "tschüß3");
create_verify_dir($prefix, "Voláçao3");
create_verify_dir($prefix, "привет3");
create_verify_dir($prefix, "テストマルチバイト・パス42");
create_verify_dir($prefix, "測試多字節路徑5");
create_verify_dir($prefix, "żółć");


$dirw = $prefix . DIRECTORY_SEPARATOR;

$old_cp = sapi_windows_cp_get();
sapi_windows_cp_set(65001);
echo "Active code page: ", sapi_windows_cp_get(), "\n";

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
sapi_windows_cp_set($old_cp);

remove_data("mb_names");

?>
--EXPECT--
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
filename: żółć : filetype: dir
filename: żółć.txt : filetype: file
filename: Ελλάδα.txt : filetype: file
filename: привет : filetype: file
filename: привет3 : filetype: dir
filename: テストマルチバイト・パス : filetype: file
filename: テストマルチバイト・パス42 : filetype: dir
filename: 測試多字節路徑 : filetype: file
filename: 測試多字節路徑5 : filetype: dir
