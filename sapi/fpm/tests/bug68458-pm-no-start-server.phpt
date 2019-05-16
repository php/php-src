--TEST--
FPM: bug68458 - Missing pm.start_servers should emit notice instead of warning
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
log_level = warning
[unconfined]
listen = {{ADDR}}
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
;pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->checkConnection();
$tester->terminate();
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
