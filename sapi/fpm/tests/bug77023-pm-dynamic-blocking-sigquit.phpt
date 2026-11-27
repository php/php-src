--TEST--
FPM: Blocked SIGQUIT prevents idle process to be killed
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
include "skipif.inc";
if (!function_exists('pcntl_sigprocmask')) die('skip Requires pcntl_sigprocmask()');
if (!getenv("RUN_RESOURCE_HEAVY_TESTS")) die("skip resource-heavy test");
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
pid = {{FILE:PID}}
[unconfined]
listen = {{ADDR}}
pm.status_path = /status
pm = dynamic
pm.max_children = 2
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 1
EOT;

$code = <<<EOT
<?php
pcntl_sigprocmask(SIG_BLOCK, [SIGQUIT, SIGTERM]);
usleep(2000000);
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start(extensions: ['pcntl']);
$tester->expectLogStartNotices();
$tester->request()->expectEmptyBody();
$tester->status([
    'total processes' => 2,
]);
$tester->expectLogWarning(
    'child \\d+ exited on signal 9 \\(SIGKILL\\) after \\d+.\\d+ seconds from start',
    'unconfined',
    timeoutSeconds: 10
);
$tester->expectLogNotice('child \\d+ started', 'unconfined');
// wait for the remaining spare process to be quit
for ($i = 0; $i < 20; $i++) {
    $status = json_decode($tester->request('json', [], '/status')->getBody('application/json'), true);
    if (($status['total processes'] ?? null) === 1) {
        break;
    }
    usleep(500000);
}
$tester->status([
    'total processes' => 1,
]);
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
