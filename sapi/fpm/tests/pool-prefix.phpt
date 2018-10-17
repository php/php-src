--TEST--
FPM: Pool prefix
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

require_once "tester.inc";

$prefix = __DIR__;

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
pid = {{FILE:PID}}
[unconfined]
prefix = $prefix
listen = {{ADDR}}
access.log = {{RFILE:LOG:ACC}}
slowlog = {{RFILE:LOG:SLOW}}
request_slowlog_timeout = 1
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogStartNotices();
$tester->ping();
$tester->expectFile(FPM\Tester::FILE_EXT_LOG_ACC, $prefix);
$tester->expectFile(FPM\Tester::FILE_EXT_LOG_ERR);
$tester->expectFile(FPM\Tester::FILE_EXT_LOG_SLOW, $prefix);
$tester->expectFile(FPM\Tester::FILE_EXT_PID);
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();
$tester->expectNoFile(FPM\Tester::FILE_EXT_PID);

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
