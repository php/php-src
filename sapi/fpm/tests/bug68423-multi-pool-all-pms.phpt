--TEST--
FPM: bug68423 - Multiple pools with different PMs (dynamic + ondemand + static)
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[pool_dynamic]
listen = {{ADDR[dynamic]}}
ping.path = /ping
ping.response = pong-dynamic
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
[pool_ondemand]
listen = {{ADDR[ondemand]}}
ping.path = /ping
ping.response = pong-on-demand
pm = ondemand
pm.max_children = 2
pm.process_idle_timeout = 10
[pool_static]
listen = {{ADDR[static]}}
ping.path = /ping
ping.response = pong-static
pm = static
pm.max_children = 2
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogStartNotices();
$tester->ping('{{ADDR[dynamic]}}', 'pong-dynamic');
$tester->ping('{{ADDR[ondemand]}}', 'pong-on-demand');
$tester->ping('{{ADDR[static]}}', 'pong-static');
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
