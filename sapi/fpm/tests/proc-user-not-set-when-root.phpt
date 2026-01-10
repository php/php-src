--TEST--
FPM: Process user setting unset when running as root
--SKIPIF--
<?php
include "skipif.inc";
FPM\Tester::skipIfNotRoot();
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg);
$tester->start(envVars: [
    'TEST_FPM_RUN_AS_ROOT' => 0,
]);
$tester->expectLogAlert(
    "'user' directive has not been specified when running as a root without --allow-to-run-as-root",
    'unconfined'
);
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
