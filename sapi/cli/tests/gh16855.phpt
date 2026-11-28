--TEST--
GH-16855: --loglevel CLI option for the built-in server
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');

echo "=== help ===\n";
$help = shell_exec("$php -n -h");
echo str_contains($help, '--loglevel') ? "help lists --loglevel\n" : "missing --loglevel\n";

echo "=== invalid value ===\n";
passthru("$php -n -S localhost:0 --loglevel 9 2>&1", $status);
echo "exit=$status\n";

echo "=== without -S ===\n";
passthru("$php -n --loglevel 1 -r 'echo 1;' 2>&1", $status);
echo "exit=$status\n";

echo "=== loglevel 1 omits request logs ===\n";
$quiet = php_cli_server_start('echo "ok";', 'index.php', ['--loglevel', '1']);
file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS);
$out = file_get_contents($quiet->outputFile);
echo str_contains($out, 'Development Server') ? "startup logged\n" : "startup missing\n";
echo (str_contains($out, 'Accepted') || str_contains($out, ' GET ') || str_contains($out, 'Closing'))
    ? "request logs present\n"
    : "request logs omitted\n";
?>
--EXPECT--
=== help ===
help lists --loglevel
=== invalid value ===
Invalid --loglevel "9". Use 1 (process), 2 (error), or 3 (message).
exit=1
=== without -S ===
--loglevel is only valid with the built-in server (-S).
exit=1
=== loglevel 1 omits request logs ===
startup logged
request logs omitted
