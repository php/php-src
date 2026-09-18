--TEST--
GHSA-9f67-6fw4-hpfp: Windows reserved device names are rejected in filesystem paths
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) !== "WIN") {
    die("skip Windows only");
}
?>
--FILE--
<?php

$dir = sys_get_temp_dir() . DIRECTORY_SEPARATOR . 'ghsa-9f67-6fw4-hpfp-' . bin2hex(random_bytes(4));
mkdir($dir);

echo "Bare device names keep working:\n";
foreach (['NUL', 'nul:', '\\\\.\\NUL'] as $name) {
    $h = @fopen($name, 'wb');
    var_dump(is_resource($h));
    if ($h) {
        fclose($h);
    }
}

echo "Reserved names in paths are rejected:\n";
foreach (['NUL', 'con', 'COM1', 'LPT9', 'CONIN$', 'NUL.txt', 'con.php', 'aux:stream', 'NUL .txt', 'NUL :txt', '.\\NUL'] as $name) {
    $blocked = $dir . DIRECTORY_SEPARATOR . $name;
    var_dump(@fopen($blocked, 'wb') === false);
    var_dump(file_exists($blocked));
}
var_dump(@fopen('NUL.txt', 'wb') === false);
var_dump(@stat($dir . DIRECTORY_SEPARATOR . 'NUL') === false);
var_dump(@mkdir($dir . DIRECTORY_SEPARATOR . 'NUL') === false);
var_dump(@rename(__FILE__, $dir . DIRECTORY_SEPARATOR . 'PRN') === false);
var_dump(@file_put_contents($dir . DIRECTORY_SEPARATOR . 'CON', 'x') === false);

echo "Similar names are fine:\n";
foreach (['NULL', 'CON1', 'COM10', 'LPT0', 'console.txt', 'x.nul', 'aux_'] as $name) {
    $file = $dir . DIRECTORY_SEPARATOR . $name;
    var_dump(file_put_contents($file, 'x') === 1 && file_exists($file));
    unlink($file);
}

rmdir($dir);

?>
--EXPECT--
Bare device names keep working:
bool(true)
bool(true)
bool(true)
Reserved names in paths are rejected:
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Similar names are fine:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
