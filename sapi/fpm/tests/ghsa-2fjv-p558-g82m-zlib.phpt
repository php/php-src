--TEST--
FPM: late zlib shutdown error does not recreate the fatal error backtrace
--EXTENSIONS--
zlib
--SKIPIF--
<?php include "skipif.inc"; ?>
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
echo getmypid(), "\n", date_default_timezone_get();
PHP;

$testDir = __DIR__ . '/ghsa-2fjv-p558-g82m-zlib-data';
mkdir($testDir);
file_put_contents($testDir . '/producer.php', $code);
file_put_contents($testDir . '/php.ini', "[PATH=$testDir]\nzlib.output_compression=0\noutput_handler=ob_gzhandler\n");

$tester = new FPM\Tester($cfg, $code);
$tester->start(extraArgs: ['-c', $testDir . '/php.ini'], extensions: ['zlib'], iniEntries: [
    'fatal_error_backtraces' => '1',
    'display_errors' => '0',
    'log_errors' => '1',
    'date.timezone' => 'UTC',
    'zlib.output_compression' => '1',
]);
$tester->expectLogStartNotices();
$body = $tester->request(address: '{{ADDR:UDS}}')->expectError(null)->getBody();
var_dump(preg_match('/^\d+\nUTC$/D', $body) === 1);

// Restore compression before output_handler to raise a late shutdown error.
$tester->request(address: '{{ADDR:UDS}}', scriptFilename: $testDir . '/producer.php')
->expectBody($body)->expectError(
    'PHP message: PHP Fatal error:  PHP Request Shutdown: '
        . 'Cannot use both zlib.output_compression and output_handler together!! in Unknown on line 0'
);

// This diagnostic occurs before the next request's executor initialization.
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
$testDir = __DIR__ . '/ghsa-2fjv-p558-g82m-zlib-data';
@unlink($testDir . '/producer.php');
@unlink($testDir . '/php.ini');
@rmdir($testDir);
?>
