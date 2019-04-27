--TEST--
FPM: Status basic test
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = static 
pm.max_children = 1
pm.status_path = /status
EOT;

$expectedStatusData = [
    'pool'                 => 'unconfined',
    'process manager'      => 'static',
    'listen queue'         => 0,
    'max listen queue'     => 0,
    'idle processes'       => 0,
    'active processes'     => 1,
    'total processes'      => 1,
    'max active processes' => 1,
    'max children reached' => 0,
    'slow requests'        => 0,
];

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectEmptyBody();
$tester->status($expectedStatusData);
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
