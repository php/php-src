--TEST--
FPM: GH-8885 - access.log with stderr begins to write logs to error_log after reloading logs
--SKIPIF--
<?php
include "skipif.inc";
FPM\Tester::skipIfRoot();
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
access.log=/dev/stderr
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg);
$tester->start(forceStderr: false);
$tester->expectNoLogMessages();
$tester->switchLogSource('{{FILE:LOG}}');
$tester->expectLogStartNotices();
$tester->ping();
$tester->switchLogSource('{{MASTER:OUT}}');
$tester->expectLogPattern('/127.0.0.1 .* "GET \/ping" 200/');
$tester->reloadLogs();
$tester->switchLogSource('{{FILE:LOG}}');
$tester->expectLogReloadingLogsNotices();
$tester->ping();
$tester->switchLogSource('{{MASTER:OUT}}');
$tester->expectLogPattern('/127.0.0.1 .* "GET \/ping" 200/');
$tester->switchLogSource('{{FILE:LOG}}');
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->switchLogSource('{{MASTER:OUT}}');
$tester->expectNoLogMessages();
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
