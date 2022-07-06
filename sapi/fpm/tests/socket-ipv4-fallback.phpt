--TEST--
FPM: Socket port connection falls back to IPv4
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
listen = {{PORT}}
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg);
$port = $tester->getPort();
// Occupy our port and let things fail
$busy = stream_socket_server("tcp://[::]:$port");
$tester->start();
$tester->expectLogNotice('Failed implicitly binding to ::, retrying with 0.0.0.0');
$tester->expectLogError("unable to bind listening socket for address '$port': " .
    'Address already in use \(\d+\)');
$tester->expectLogError('FPM initialization failed');
$tester->close(true);
?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
