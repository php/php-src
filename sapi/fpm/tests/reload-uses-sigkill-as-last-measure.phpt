--TEST--
If SIGQUIT and SIGTERM during reloading fail, SIGKILL should be sent
--SKIPIF--
<?php
include "skipif.inc";
if (!function_exists('pcntl_sigprocmask')) die('skip Requires pcntl_sigprocmask()');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
pid = {{FILE:PID}}
process_control_timeout=1
[unconfined]
listen = {{ADDR}}
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 1
EOT;

$code = <<<EOT
<?php
pcntl_sigprocmask(SIG_BLOCK, [SIGQUIT, SIGTERM]);
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectEmptyBody();
$tester->signal('USR2');
$tester->expectLogNotice('Reloading in progress ...');
$tester->expectLogNotice('reloading: .*');
// The timeout needs to elapse twice until the process will be killed,
// so we have to wait at least two seconds.
sleep(2);
$tester->expectLogNotice('using inherited socket fd=\d+, "127.0.0.1:\d+"');
$tester->expectLogStartNotices();
$tester->ping('{{ADDR}}');
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
