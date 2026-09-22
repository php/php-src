--TEST--
GHSA-62xp-839h-2637: IPv6 allowed client compared on all 128 bits
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
[exact]
listen = {{ADDR:IPv6:ANY[exact]}}
listen.allowed_clients = ::1
pm = static
pm.max_children = 1
[prefix]
listen = {{ADDR:IPv6:ANY[prefix]}}
listen.allowed_clients = ::dead:beef
pm = static
pm.max_children = 1
catch_workers_output = yes
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogStartNotices();
$tester->checkRequest('{{ADDR:IPv6[exact]}}', 'exact: ok', 'exact: error');
$tester->checkRequest('{{ADDR:IPv6[prefix]}}', 'prefix: ok', 'prefix: error');
$tester->terminate();
$tester->expectLogWarning(
    'child %d said into stderr: "ERROR: Connection disallowed: IP address \'::1\' has been dropped."',
    'prefix'
);
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECT--
exact: ok
prefix: error
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
