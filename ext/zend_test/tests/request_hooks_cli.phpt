--TEST--
Request hooks begin for command line code, not when linting or when no code runs
--EXTENSIONS--
zend_test
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$args = getenv('TEST_PHP_EXTRA_ARGS') . ' -d zend_test.print_request_hooks=1';
$empty = __DIR__ . '/request_hooks_cli.empty';
file_put_contents($empty, '');

echo shell_exec("$php $args -l " . escapeshellarg(__FILE__));
echo shell_exec("$php $args -r " . escapeshellarg('echo 1, PHP_EOL;'));
echo shell_exec("echo input | $php $args -R " . escapeshellarg('echo $argn, PHP_EOL;'));
echo "empty input\n";
echo shell_exec("$php $args -R " . escapeshellarg('echo $argn, PHP_EOL;') . ' < ' . escapeshellarg($empty));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/request_hooks_cli.empty');
?>
--EXPECTF--
No syntax errors detected in %s
request_shutdown
request_begin
1
request_end
request_shutdown
request_begin
input
request_end
request_shutdown
empty input
request_shutdown
