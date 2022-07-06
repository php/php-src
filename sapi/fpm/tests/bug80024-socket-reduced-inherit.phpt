--TEST--
FPM: bug80024 - Duplication of info about inherited socket after pool removing
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg['main'] = <<<EOT
[global]
error_log = {{FILE:LOG}}
pid = {{FILE:PID}}
include = {{INCLUDE:CONF}}
EOT;

$cfg['poolTemplate'] = <<<EOT
[pool_%index%]
listen = {{ADDR:UDS[pool_%index%]}}
pm = ondemand
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
pm.max_children = 5
EOT;

$cfg['count'] = 129;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogStartNotices();
$cfg['count'] = 128;
$tester->reload($cfg);
$tester->expectLogReloadingNotices(129);
$tester->reload();
$tester->expectLogReloadingNotices(128);
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
