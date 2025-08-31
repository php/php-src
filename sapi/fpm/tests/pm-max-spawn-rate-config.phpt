--TEST--
FPM: set pm.max_spawn_rate
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
log_level = notice
[unconfined]
listen = {{ADDR}}
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
pm.max_spawn_rate = 64
EOT;

$tester = new FPM\Tester($cfg);
$tester->start(['-tt']);
$tester->expectLogConfigOptions(['pm.max_spawn_rate = 64']);
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
