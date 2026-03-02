--TEST--
FPM: GH-16932 - scoreboard fields are reset after the request
--EXTENSIONS--
pcntl
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
pid = {{FILE:PID}}
[unconfined]
listen = {{ADDR}}
pm.status_path = /status
pm = dynamic
pm.max_children = 2
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 1
EOT;

$code = <<<EOT
<?php
echo "hi!";
EOT;


$tester = new FPM\Tester($cfg, $code);
$tester->start(extensions: ['pcntl']);
$tester->expectLogStartNotices();
$tester->request();
$tester->request();
$tester->request();
$tester->request();
$tester
    ->request(uri: '/status', query: 'json')
    ->expectJsonBodyPatternForStatusField('accepted conn', '5');
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
