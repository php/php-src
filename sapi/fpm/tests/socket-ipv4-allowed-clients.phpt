--TEST--
FPM: Socket for IPv4 allowed client only
--SKIPIF--
<?php
include "skipif.inc";
FPM\Tester::skipIfIPv6IsNotSupported();
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR:IPv6:ANY}}
listen.allowed_clients = 127.0.0.1
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogStartNotices();
$tester->checkRequest('127.0.0.1', 'IPv4: ok', 'IPv4: error');
$tester->checkRequest('[::1]', 'IPv6: ok', 'IPv6: error');
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECT--
IPv4: ok
IPv6: error
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
