--TEST--
Bug #77812 (Interactive mode does not support PHP 7.3-style heredoc)
--SKIPIF--
<?php
if (!extension_loaded('readline')) die('skip readline extension not available');
if (READLINE_LIB !== "libedit") die('skip libedit only');
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');
$ini = getenv('TEST_PHP_EXTRA_ARGS');
$descriptorspec = [['pipe', 'r'], STDOUT, STDERR];
$proc = proc_open("$php $ini -a", $descriptorspec, $pipes);
var_dump($proc);
fwrite($pipes[0], "echo <<<FOO\n    bar\n    FOO;\n");
fwrite($pipes[0], "print(<<<FOO\nxx\nFOO);\n");
fwrite($pipes[0], "echo <<<FOO\n    xxx\n    FOO;\nFOO\n;\n");
fwrite($pipes[0], "echo <<<FOO\nFOOL\nFOO\n,1;\n");
fwrite($pipes[0], "echo <<<FOO\nFOO4\nFOO\n,2;\n");
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECTF--
resource(%d) of type (process)
Interactive shell

bar
xx
xxx

Warning: Use of undefined constant FOO - assumed 'FOO' (this will throw an Error in a future version of PHP) in php shell code on line %d
FOOL1
FOO42
