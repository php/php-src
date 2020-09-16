--TEST--
FPM: UDS connection, expand $pool and create socket directory
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$dir = __DIR__ . '/' . basename(__FILE__, '.php') . '-$pool';
$sock = "$dir/fpm.sock";
$dir = __DIR__ . '/' . basename(__FILE__, '.php') . '-default';
$sock_exp = "$dir/fpm.sock";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[default]
listen = $sock
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogStartNotices();
$tester->ping($sock_exp);
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

var_dump(rmdir($dir));

?>
Done
--EXPECT--
bool(true)
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
