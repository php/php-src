--TEST--
Interactive shell: auto_prepend_file is executed before input
--EXTENSIONS--
readline
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$prepend = tempnam(sys_get_temp_dir(), 'readline_cli_prepend');
file_put_contents($prepend, <<<'PHP'
<?php
echo "prepended\n";
define('READLINE_CLI_PREPENDED', 'ok');
readline_completion_function(static fn() => []);
PHP);

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$ini = getenv('TEST_PHP_EXTRA_ARGS');
$descriptorspec = [['pipe', 'r'], STDOUT, STDERR];
$proc = proc_open("$php $ini -d auto_prepend_file=" . escapeshellarg($prepend) . " -a", $descriptorspec, $pipes);
fwrite($pipes[0], "echo READLINE_CLI_PREPENDED . \"\n\";\n");
fwrite($pipes[0], "exit\n");
fclose($pipes[0]);
proc_close($proc);
unlink($prepend);
?>
--EXPECTF--
%AInteractive shell%Aprepended%Aok%A
