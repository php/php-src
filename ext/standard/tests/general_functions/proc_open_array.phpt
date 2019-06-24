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

echo "\nCheck that arguments are correctly passed through:\n";
$args = [
    "Simple",
    "White space\ttab\nnewline",
    "Nul\0bytes", // Should be cut off
    '"Quoted"',
    'Qu"ot"ed',
    '\\Back\\slash\\',
    '\\\\Back\\\\slash\\\\',
    '\\"Qu\\"ot\\"ed\\"',
];
$cmd = [$php, '-r', 'var_export(array_slice($argv, 1));', '--', ...$args];
$proc = proc_open($cmd, $ds, $pipes);
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

Check that arguments are correctly passed through:
array (
  0 => 'Simple',
  1 => 'White space	tab
newline',
  2 => 'Nul',
  3 => '"Quoted"',
  4 => 'Qu"ot"ed',
  5 => '\\Back\\slash\\',
  6 => '\\\\Back\\\\slash\\\\',
  7 => '\\"Qu\\"ot\\"ed\\"',
)
