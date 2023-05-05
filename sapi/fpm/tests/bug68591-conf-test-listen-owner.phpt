--TEST--
FPM: bug68591 - config test listen owner existence
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
[unconfined]
listen = {{ADDR:UDS}}
listen.owner = aaaaaa
user = root
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg);
$tester->testConfig();

?>
Done
--EXPECTF--
ERROR: [pool unconfined] cannot get uid for user 'aaaaaa': %s
ERROR: FPM initialization failed
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
