--TEST--
FPM: gh68591 - daemonized mode duplicated logs
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = /dev/stderr
daemonize = true
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = 1
EOT;

$tester = new FPM\Tester($cfg);
$tester->testConfig(dumpConfig: false, printOutput: true);

?>
Done
--EXPECTF--
%sNOTICE: configuration file %s test is successful
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
