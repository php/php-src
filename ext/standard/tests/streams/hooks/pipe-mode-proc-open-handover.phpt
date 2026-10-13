--TEST--
IO hooks: a pipe handed to a proc_open child is blocking again once PHP closed its end
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip POSIX only');
?>
--FILE--
<?php
require __DIR__ . '/pipe-mode.inc';

$writer = proc_open([PHP_BINARY, '-n', '-r', 'echo "one\n"; fgets(STDIN); echo "two\n";'],
    [0 => ['pipe', 'r'], 1 => ['pipe', 'w']], $wpipes);
stream_set_read_buffer($wpipes[1], 0);
var_dump(pipe_mode_read_hooked($wpipes[1], 4));

// Reads its stdin, the writer's output, once the gate opens
$reader = proc_open([PHP_BINARY, '-n', '-r', 'fgets(fopen("php://fd/3", "r")); var_dump(fgets(STDIN));'],
    [0 => $wpipes[1], 1 => ['pipe', 'w'], 3 => ['pipe', 'r']], $rpipes);
fclose($wpipes[1]);
fwrite($rpipes[3], "go\n");
usleep(50000);
fwrite($wpipes[0], "go\n");
echo stream_get_contents($rpipes[1]);

fclose($rpipes[3]);
fclose($rpipes[1]);
proc_close($reader);
fclose($wpipes[0]);
proc_close($writer);
?>
--EXPECT--
string(4) "one
"
string(4) "two
"
