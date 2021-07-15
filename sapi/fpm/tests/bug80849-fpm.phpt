--TEST--
Bug #80849 (HTTP Status header truncation)
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
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$code = <<<EOT
<?php
header('HTTP/1.1 201 ' . str_repeat('A', 1014), true);
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester
    ->request()
    ->expectHeader('Status', '201 ' . str_repeat('A', 1011));
$tester->terminate();
$tester->close();
?>
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
--EXPECT--
