--TEST--
Bug #79976 (anonymous class instanceof error in cli)
--SKIPIF--
<?php
if (!extension_loaded('readline')) die('skip readline extension not available');
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');
$ini = getenv('TEST_PHP_EXTRA_ARGS');
$descriptorspec = [['pipe', 'r'], STDOUT, STDERR];
$proc = proc_open("$php $ini -a", $descriptorspec, $pipes);
var_dump($proc);
fwrite($pipes[0], "class A {}\n");
fwrite($pipes[0], "class B {}\n");
fwrite($pipes[0], "var_dump((new class() extends A {}) instanceof A);\n");
fwrite($pipes[0], "var_dump((new class() extends B {}) instanceof B);\n");
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECT--
resource(5) of type (process)
Interactive shell

bool(true)
bool(true)
