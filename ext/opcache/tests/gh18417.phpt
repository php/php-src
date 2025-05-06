--TEST--
GH-18417 (Windows SHM reattachment fails when increasing memory_consumption or jit_buffer_size)
--SKIPIF--
<?php
$extDir = ini_get('extension_dir');
if (!file_exists($extDir . '/php_opcache.dll')) {
    die('skip Opcache DLL not found in extension_dir (Windows-only)');
}
?>
--FILE--
<?php
$memory_consumption = (int) ini_get("opcache.memory_consumption");
$new_memory_consumption = $memory_consumption * 2;
$extension_dir = ini_get("extension_dir");

$descriptorspec = [
    0 => ["pipe", "r"],
    1 => ["pipe", "w"],
    2 => ["pipe", "w"],
];

$proc = proc_open([
    PHP_BINARY,
    "-n",
    "-d", "extension_dir=$extension_dir",
    "-d", "zend_extension=opcache",
    "-d", "opcache.memory_consumption=$new_memory_consumption",
    "-d", "opcache.enable=1",
    "-d", "opcache.enable_cli=1",
    "-r",
    "echo 1;"
], $descriptorspec, $pipes);

echo stream_get_contents($pipes[1]);
echo stream_get_contents($pipes[2]);

proc_close($proc);
?>
--EXPECT--
1
