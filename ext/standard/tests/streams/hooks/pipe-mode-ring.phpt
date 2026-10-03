--TEST--
IO hooks: a blocking pipe the ring saw stays blocking without a provider and is given back blocking
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip POSIX only');
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php
require __DIR__ . '/pipe-mode.inc';

$code = 'echo "A"; fgets(STDIN); usleep(100000); echo "B";';
$proc = proc_open([PHP_BINARY, '-n', '-r', $code], [0 => ['pipe', 'r'], 1 => ['pipe', 'w']], $pipes);
stream_set_read_buffer($pipes[1], 0);
$probe = pipe_mode_probe($pipes[1]);
echo "before: ", pipe_mode_ask($probe), "\n";

var_dump(pipe_mode_read_hooked($pipes[1], 1, new Io\Ring\Engine()));
echo "switched: ", pipe_mode_ask($probe), "\n";

var_dump(stream_get_meta_data($pipes[1])['blocked']);
fwrite($pipes[0], "go\n");
var_dump(fread($pipes[1], 1));

fclose($pipes[1]);
echo "closed: ", pipe_mode_ask($probe), "\n";

pipe_mode_probe_close($probe);
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECT--
before: 1
string(1) "A"
switched: 0
bool(true)
string(1) "B"
closed: 1
