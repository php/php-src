--TEST--
curl_exec() doesn't reach hard limit
--EXTENSIONS--
curl
--FILE--
<?php
require __DIR__ . '/server.inc';
$host = curl_cli_server_start();

$extension = ini_get('extension_dir') . '/' . (PHP_OS_FAMILY === 'Windows' ? 'php_' : '') . 'curl.' . PHP_SHLIB_SUFFIX;
$args = [];
if (is_file($extension)) {
    $args[] = '-d';
    $args[] = "extension=$extension";
}

$code = <<<'PHP'
$ch = curl_init($argv[1]);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_exec($ch);
echo "no OOM\n";
PHP;

$php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
$p = proc_open(
    [$php, '-n', ...$args, '-d', 'memory_limit=8M', '-d', 'fatal_error_backtraces=Off', '-r', $code, "http://$host/get.inc?test=bigbody&mib=16"],
    [1 => ['pipe', 'w'], 2 => ['pipe', 'w']],
    $pipes
);
echo stream_get_contents($pipes[1]);
echo stream_get_contents($pipes[2]);
fclose($pipes[1]);
fclose($pipes[2]);
proc_close($p);
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
