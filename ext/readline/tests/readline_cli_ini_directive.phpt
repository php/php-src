--TEST--
Interactive shell: setting an INI directive via #name=value
--EXTENSIONS--
readline
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');
$ini = getenv('TEST_PHP_EXTRA_ARGS');
$descriptorspec = [['pipe', 'r'], STDOUT, STDERR];
$proc = proc_open("$php $ini -a", $descriptorspec, $pipes);
fwrite($pipes[0], "#precision=5\n");
fwrite($pipes[0], "echo 'INI[' . ini_get('precision') . ']';\n");
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECTF--
%AINI[5]%A
