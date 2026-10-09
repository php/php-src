--TEST--
GH-24148: Bare NUL remains usable with open_basedir on Windows
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
if (!function_exists('proc_open')) die('skip proc_open unavailable');
?>
--INI--
open_basedir=
--FILE--
<?php
chdir(__DIR__);
foreach ([
    'Directory allowed' => __DIR__,
    'Devices explicitly allowed' => __FILE__ . ';NUL;NUL:',
    'Only file allowed' => __FILE__,
] as $label => $basedir) {
    echo "$label:\n";
    var_dump(ini_set('open_basedir', $basedir) !== false);
    foreach (['NUL', 'nul:'] as $name) {
        $stream = @fopen($name, 'c');
        var_dump(is_resource($stream));
        if (is_resource($stream)) {
            fclose($stream);
        }
    }
    $process = @proc_open('cmd /c exit 0', [
        ['pipe', 'r'], ['file', 'NUL', 'w'], ['file', 'nul:', 'w'],
    ], $pipes);
    if (is_resource($process)) {
        fclose($pipes[0]);
        var_dump(proc_close($process) === 0);
    } else {
        var_dump(false);
    }
    var_dump(file_get_contents(__FILE__) !== false);
}
?>
--EXPECT--
Directory allowed:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Devices explicitly allowed:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Only file allowed:
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
