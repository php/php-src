--TEST--
FPM: cpu affinity test
--SKIPIF--
<?php include "skipif.inc";

if (!str_contains(PHP_OS, 'Linux') && !str_contains(PHP_OS, 'FreeBSD')) {
    die('skipped supported only on Linux and FreeBSD');
}

if (str_contains(PHP_OS, 'Linux')) {
    $cmd = 'nproc';
} else {
    $cmd = 'sysctl hw.ncpus';
}

$nproc = intval(exec($cmd));
if ($nproc < 2) {
    die('skipped supported only on multicores environments');
}
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
pid = {{FILE:PID}}
[unconfined]
listen = {{ADDR:IPv4}}
pm = dynamic
pm.max_children = 1
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 1
process.cpu_list = 1
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogStartNotices();
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
