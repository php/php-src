--TEST--
Interactive results in readline disabled
--INI--
cli.enable_interactive_shell_result_function=0
--SKIPIF--
<?php
if (!extension_loaded('readline')) die('skip readline extension not available');
if (READLINE_LIB !== 'readline') { die('skip readline only'); }
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');
$ini = getenv('TEST_PHP_EXTRA_ARGS');
$descriptorspec = [['pipe', 'r'], STDOUT, STDERR];
$proc = proc_open("$php $ini -a", $descriptorspec, $pipes);
var_dump($proc);

// var_dump, json_encode, or any more complex dumping can be used in readline_interactive_shell_result_function.
$write_line = fn(string $line) => fwrite($pipes[0], $line . "\n");
// Result expressions aren't dumped when this functionality is disabled.
$write_line('1+1;');
$write_line('printf("Test\n");');
// Calling readline_interactive_shell_result_function is deliberately a no-op when this functionality
// is disabled.
$write_line(<<<'EOT'
readline_interactive_shell_result_function(
    function(string $code, $result) { var_dump($result); });
EOT);
$write_line('1+1;');
$write_line('printf("Test\n");');
fclose($pipes[0]);
proc_close($proc);
?>
--EXPECTF--
resource(5) of type (process)
Interactive shell

php > 1+1;
php > printf("Test\n");
Test
php > readline_interactive_shell_result_function(
php (     function(string $code, $result) { var_dump($result); });
php > 1+1;
php > printf("Test\n");
Test
php >
