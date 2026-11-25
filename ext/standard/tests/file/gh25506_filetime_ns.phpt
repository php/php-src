--TEST--
fileatime_ns(), filemtime_ns(), and filectime_ns() return nanosecond timestamps
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux' || !function_exists('proc_open')) {
    die('skip requires proc_open() and GNU touch');
}
$process = proc_open(['touch', '--version'], [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
if (!is_resource($process)) {
    die('skip GNU touch is unavailable');
}
fclose($pipes[0]);
$output = stream_get_contents($pipes[1]);
$error = stream_get_contents($pipes[2]);
fclose($pipes[1]);
fclose($pipes[2]);
if (proc_close($process) !== 0) {
    die('skip GNU touch is unavailable');
}
$filename = tempnam(sys_get_temp_dir(), 'filetime_ns');
$process = proc_open(['touch', '-m', '-d', '@1234567890.123456789', $filename], [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
fclose($pipes[0]);
fclose($pipes[1]);
fclose($pipes[2]);
if (proc_close($process) !== 0) {
    unlink($filename);
    die('skip GNU touch cannot set nanosecond timestamps');
}
$process = proc_open(['stat', '--format=%y', $filename], [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
fclose($pipes[0]);
$output = stream_get_contents($pipes[1]);
$error = stream_get_contents($pipes[2]);
fclose($pipes[1]);
fclose($pipes[2]);
$status = proc_close($process);
unlink($filename);
if ($status !== 0 || !str_contains($output, '.123456789')) {
    die('skip filesystem does not retain nanosecond timestamps');
}
?>
--FILE--
<?php
$filename = tempnam(sys_get_temp_dir(), 'filetime_ns');
$process = proc_open(['touch', '-a', '-d', '@1234567890.123456789', $filename], [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
fclose($pipes[0]);
fclose($pipes[1]);
fclose($pipes[2]);
proc_close($process);
var_dump(fileatime_ns($filename) === 1234567890123456789);

$process = proc_open(['touch', '-m', '-d', '@1234567890.123456789', $filename], [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
fclose($pipes[0]);
fclose($pipes[1]);
fclose($pipes[2]);
proc_close($process);
clearstatcache();
var_dump(filemtime_ns($filename) === 1234567890123456789);

$process = proc_open(['touch', '-a', '-d', '@-1.123456789', $filename], [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
fclose($pipes[0]);
fclose($pipes[1]);
fclose($pipes[2]);
proc_close($process);
clearstatcache();
var_dump(fileatime_ns($filename) === -1123456789);

foreach (['atime', 'mtime', 'ctime'] as $time) {
    $seconds = call_user_func('file' . $time, $filename);
    $nanoseconds = call_user_func('file' . $time . '_ns', $filename);
    var_dump(is_int($nanoseconds) || is_float($nanoseconds));
    var_dump((int) floor($nanoseconds / 1_000_000_000) === $seconds);
}

unlink($filename);
var_dump(@filemtime_ns($filename));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
