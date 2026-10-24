--TEST--
FPM: fatal error backtrace is reset before the next request's INI diagnostics
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = 1
env[ZEND_ASAN_FORCE_FAST_SHUTDOWN] = 1
EOT;

$code = <<<'PHP'
<?php
echo getmypid(), "\n";
if (isset($_GET['fatal'])) {
    function trigger_fatal() {
        eval("class RequestReset {}; class RequestReset {}");
    }
    trigger_fatal();
}
echo date_default_timezone_get(), "\n";
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'fatal_error_backtraces' => '1',
    'display_errors' => '0',
    'log_errors' => '1',
    'date.timezone' => 'UTC',
]);
$tester->expectLogStartNotices();
$response = $tester->request('fatal=1')->expectErrorPattern(
    '/\APHP message: PHP Fatal error:  Cannot redeclare class RequestReset\b[^\n]*\n'
        . 'Stack trace:\n#0 [^\n]*: eval\(\)\n'
        . '#1 [^\n]*: trigger_fatal\(\)\n#2 \{main\}\z/'
);
$pid = $response->getBody();
var_dump(preg_match('/^\d+$/D', $pid) === 1);

/* PHP_VALUE is applied before php_request_startup(), while shutdown is still flagged. */
$tester->request(headers: ['PHP_VALUE' => 'date.timezone=invalid'])
    ->expectBody($pid . "\nUTC")
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
?>
