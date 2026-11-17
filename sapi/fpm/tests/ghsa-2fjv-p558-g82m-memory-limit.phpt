--TEST--
FPM: late memory limit shutdown error does not recreate the fatal error backtrace
--SKIPIF--
<?php
include "skipif.inc";
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR:UDS}}
pm = static
pm.max_children = 1
EOT;

$code = <<<'PHP'
<?php
if ($_SERVER['QUERY_STRING'] === 'producer') {
    // The last error message is only freed after the INI entries are restored,
    // so this keeps the allocation alive even without fast shutdown (debug builds).
    @trigger_error(str_repeat('x', 20 * 1024 * 1024));
}
echo getmypid(), "\n", date_default_timezone_get();
PHP;

$testDir = __DIR__ . '/ghsa-2fjv-p558-g82m-memory-limit-data';
mkdir($testDir);
file_put_contents($testDir . '/producer.php', $code);
file_put_contents($testDir . '/php.ini', "[PATH=$testDir]\nmax_memory_limit=64M\nmemory_limit=64M\n");

$tester = new FPM\Tester($cfg, $code);
$tester->start(extraArgs: ['-c', $testDir . '/php.ini'], iniEntries: [
    'fatal_error_backtraces' => '1',
    'display_errors' => '0',
    'log_errors' => '1',
    'date.timezone' => 'UTC',
    'max_memory_limit' => '8M',
    'memory_limit' => '8M',
]);
$tester->expectLogStartNotices();
$body = $tester->request(address: '{{ADDR:UDS}}')->expectError(null)->getBody();
var_dump(preg_match('/^\d+\nUTC$/D', $body) === 1);

// Restoring the global maximum fails while the request allocation still exists.
$tester->request(query: 'producer', address: '{{ADDR:UDS}}', scriptFilename: $testDir . '/producer.php')
->expectBody($body)->expectErrorPattern(
    '/\APHP message: PHP Fatal error:  Failed to set memory limit to 8388608 bytes '
        . '\(Current memory usage is \d+ bytes\) in Unknown on line 0\z/'
);

$tester->request(address: '{{ADDR:UDS}}', headers: ['PHP_VALUE' => 'date.timezone=invalid'])
    ->expectBody($body)
    ->expectError("PHP message: PHP Warning:  PHP Request Shutdown: "
        . "Invalid date.timezone value 'invalid', using 'UTC' instead in Unknown on line 0");
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();
?>
Done
--EXPECT--
bool(true)
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
$testDir = __DIR__ . '/ghsa-2fjv-p558-g82m-memory-limit-data';
@unlink($testDir . '/producer.php');
@unlink($testDir . '/php.ini');
@rmdir($testDir);
?>
