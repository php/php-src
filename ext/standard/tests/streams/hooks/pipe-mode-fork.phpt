--TEST--
IO hooks: a forked child closing its copy of a pipe leaves the parent's descriptor non-blocking
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip POSIX only');
?>
--FILE--
<?php
require __DIR__ . '/pipe-mode.inc';

$proc = proc_open([PHP_BINARY, '-n', '-r', 'echo "AB"; fgets(STDIN); echo "C";'], [0 => ['pipe', 'r'], 1 => ['pipe', 'w']], $pipes);
stream_set_read_buffer($pipes[1], 0);
$probe = pipe_mode_probe($pipes[1]);

var_dump(pipe_mode_read_hooked($pipes[1], 1));
echo "switched: ", pipe_mode_ask($probe), "\n";

$pid = pcntl_fork();
if ($pid === 0) {
    fclose($pipes[1]);
    exit(0);
}
pcntl_waitpid($pid, $status);
echo "child closed: ", pipe_mode_ask($probe), "\n";

var_dump(pipe_mode_read_hooked($pipes[1], 1));
fwrite($pipes[0], "go\n");
var_dump(fread($pipes[1], 1));

fclose($pipes[1]);
echo "parent closed: ", pipe_mode_ask($probe), "\n";

pipe_mode_probe_close($probe);
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECT--
string(1) "A"
switched: 0
child closed: 0
string(1) "B"
string(1) "C"
parent closed: 1
