--TEST--
Using proc_open() with a command array (no shell)
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');
$ds = [
    0 => ['pipe', 'r'],
    1 => ['pipe', 'w'],
    2 => ['pipe', 'w'],
];

echo "Empty command array:";
$proc = proc_open([], $ds, $pipes);

echo "\nBasic usage:\n";
$proc = proc_open([$php, '-r', 'echo "Hello World!\n";'], $ds, $pipes);
fpassthru($pipes[1]);
proc_close($proc);

putenv('ENV_1=ENV_1');
$env = ['ENV_2' => 'ENV_2'];
$cmd = [$php, '-r', 'var_dump(getenv("ENV_1"), getenv("ENV_2"));'];

echo "\nEnvironment inheritance:\n";
$proc = proc_open($cmd, $ds, $pipes);
fpassthru($pipes[1]);
proc_close($proc);

echo "\nExplicit environment:\n";
$proc = proc_open($cmd, $ds, $pipes, null, $env);
fpassthru($pipes[1]);
proc_close($proc);

?>
--EXPECTF--
Empty command array:
Warning: proc_open(): Command array must have at least one element in %s on line %d

Basic usage:
Hello World!

Environment inheritance:
string(5) "ENV_1"
bool(false)

Explicit environment:
bool(false)
string(5) "ENV_2"
