--TEST--
FPM: cpu affinity test
--SKIPIF--
<?php include "skipif.inc";

if (!str_contains(PHP_OS, 'Linux') && !str_contains(PHP_OS, 'FreeBSD')) {
    die('skipped supported only on Linux and FreeBSD');
}
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
process.cpu_list = 12de-34ad
error_log = {{FILE:LOG}}
pid = {{FILE:PID}}
[unconfined]
listen = {{ADDR:IPv4}}
pm = dynamic
pm.max_children = 1
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 1
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogError("FPM initialization failed");
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
