--TEST--
proc_open() with cwd changes child working directory
--SKIPIF--
<?php
if (!function_exists("proc_open")) echo "skip proc_open() is not available";
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
$cwd = __DIR__ . DIRECTORY_SEPARATOR . 'proc_open_cwd_basic';
@mkdir($cwd);

$descriptors = [
	1 => ['pipe', 'w'],
	2 => ['pipe', 'w'],
];
$proc = proc_open([$php, '-n', '-r', 'echo basename(getcwd()), "\n";'], $descriptors, $pipes, $cwd);

var_dump(is_resource($proc));
echo stream_get_contents($pipes[1]);
echo stream_get_contents($pipes[2]);
fclose($pipes[1]);
fclose($pipes[2]);
var_dump(proc_close($proc));

rmdir($cwd);
?>
--CLEAN--
<?php
$cwd = __DIR__ . DIRECTORY_SEPARATOR . 'proc_open_cwd_basic';
if (is_dir($cwd)) {
	rmdir($cwd);
}
?>
--EXPECT--
bool(true)
proc_open_cwd_basic
int(0)
