--TEST--
FPM: Test extended access log limit
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$src = <<<EOT
<?php
echo "OK";
EOT;

$cfg = <<<EOT
[global]
error_log = {{RFILE:LOG:ERR}}
pid = {{RFILE:PID}}
log_limit = 2048
[unconfined]
listen = {{ADDR}}
access.log = {{RFILE:LOG:ACC}}
access.format = "'%m %r%Q%q' %s"
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

$prefix = __DIR__;
$tester = new FPM\Tester($cfg, $src);
$tester->start(['--prefix', $prefix]);
$tester->expectLogStartNotices();
$tester->request(query: 'a=' . str_repeat('a', 1500))->expectBody('OK');
$tester->expectAccessLog("'GET /log-access-extended-limit.src.php?a=" . str_repeat('a', 1500) . "' 200");
$tester->request(query: 'a=' . str_repeat('a', 2040))->expectBody('OK');
$tester->expectAccessLog("'GET /log-access-extended-limit.src.php?a=" . str_repeat('a', 2002) . '...');
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();
$tester->checkAccessLog();

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
