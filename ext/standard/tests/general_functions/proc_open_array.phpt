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

// Command array cannot be empty
$proc = proc_open([], $ds, $pipes);

$proc = proc_open([$php, '-r', 'echo "Hello World!\n";'], $ds, $pipes);
var_dump(fgets($pipes[1]));
proc_close($proc);

$env = ['FOOBARENV' => 'foobar'];
$proc = proc_open([$php, '-r', 'echo getenv("FOOBARENV");'], $ds, $pipes, null, $env);
var_dump(fgets($pipes[1]));
proc_close($proc);

?>
--EXPECTF--
Warning: proc_open(): Command array must have at least one element in %s on line %d
string(13) "Hello World!
"
string(6) "foobar"
