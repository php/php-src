--TEST--
FPM: bug68421 - IPv6 all addresses and access_log
--SKIPIF--
<?php
include "skipif.inc";
FPM\Tester::skipIfIPv6IsNotSupported();
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG:ERR}}
[unconfined]
listen = {{ADDR:IPv6:ANY}}
access.log = {{FILE:LOG:ACC}}
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
$tester->ping('127.0.0.1');
$tester->ping('[::1]');
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();
$tester->printAccessLog();
?>
Done
--EXPECTF--
127.0.0.1 %s "GET /ping" 200
::1 %s "GET /ping" 200
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
