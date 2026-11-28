--TEST--
GH-25860 (stream_copy_to_stream() from a socket with a timeout longer than INT_MAX milliseconds)
--CREDITS--
OmenP007
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$file = tempnam(sys_get_temp_dir(), 'gh25860');
$dest = fopen($file, 'wb');

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$proc = proc_open($php . ' -n -r "usleep(2000000); echo \'data\';"', [1 => ['socket']], $pipes);

stream_set_timeout($pipes[1], 4294968);
var_dump(stream_copy_to_stream($pipes[1], $dest));

fclose($pipes[1]);
fclose($dest);
proc_close($proc);
var_dump(file_get_contents($file));
@unlink($file);
?>
--EXPECT--
int(4)
string(4) "data"
