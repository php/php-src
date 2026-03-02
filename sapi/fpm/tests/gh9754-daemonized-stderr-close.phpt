--TEST--
FPM: GH-9754 - stderr is not closed in daemonized run
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
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg);
$tester->start(daemonize: true);
$tester->expectLogStartNotices();
$tester->switchLogSource('{{MASTER:OUT}}');
$tester->expectLogEmpty();
$tester->switchLogSource('{{FILE:LOG}}');
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
