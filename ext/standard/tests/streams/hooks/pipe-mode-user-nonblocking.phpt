--TEST--
IO hooks: a pipe the script made non-blocking stays non-blocking and never waits
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip POSIX only');
?>
--FILE--
<?php
require __DIR__ . '/pipe-mode.inc';

$code = 'echo "A"; fgets(STDIN);';
$proc = proc_open([PHP_BINARY, '-n', '-r', $code], [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
stream_set_read_buffer($pipes[1], 0);
stream_set_read_buffer($pipes[2], 0);
$probe1 = pipe_mode_probe($pipes[1]);
$probe2 = pipe_mode_probe($pipes[2]);

echo "-- switched by the provider, then made non-blocking\n";
var_dump(pipe_mode_read_hooked($pipes[1], 1));
var_dump(stream_set_blocking($pipes[1], false));
var_dump(stream_get_meta_data($pipes[1])['blocked']);
echo "kernel: ", pipe_mode_ask($probe1), "\n";
var_dump(pipe_mode_read_hooked($pipes[1], 1));
var_dump(fread($pipes[1], 1));

echo "-- non-blocking before any provider\n";
var_dump(stream_set_blocking($pipes[2], false));
var_dump(pipe_mode_read_hooked($pipes[2], 1));
var_dump(stream_get_meta_data($pipes[2])['blocked']);
var_dump(fread($pipes[2], 1));

echo "-- blocking again\n";
var_dump(stream_set_blocking($pipes[2], true));
var_dump(stream_get_meta_data($pipes[2])['blocked']);
echo "kernel: ", pipe_mode_ask($probe2), "\n";
var_dump(stream_set_blocking($pipes[2], false));

echo "-- the script's mode is kept at close\n";
fclose($pipes[1]);
fclose($pipes[2]);
echo "kernel: ", pipe_mode_ask($probe1), " ", pipe_mode_ask($probe2), "\n";

pipe_mode_probe_close($probe1);
pipe_mode_probe_close($probe2);
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECT--
-- switched by the provider, then made non-blocking
string(1) "A"
bool(true)
bool(false)
kernel: 0
string(0) ""
string(0) ""
-- non-blocking before any provider
bool(true)
string(0) ""
bool(false)
string(0) ""
-- blocking again
bool(true)
bool(true)
kernel: 1
bool(true)
-- the script's mode is kept at close
kernel: 0 0
