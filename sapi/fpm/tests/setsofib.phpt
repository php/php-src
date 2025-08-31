--TEST--
FPM: listen.setfib`
--SKIPIF--
<?php
include "skipif.inc";

if (!str_contains(PHP_OS, 'FreeBSD')) {
    die('skipped supported only on FreeBSD');
}
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
pid = {{FILE:PID}}
[setfib]
listen = {{ADDR}}
listen.setfib = 68000
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogError('Invalid routing table id 68000, max is 1');
$tester->terminate();
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
