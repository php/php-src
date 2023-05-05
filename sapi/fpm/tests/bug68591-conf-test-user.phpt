--TEST--
FPM: bug68591 - config test user existence
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
user = aaaaaa
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
--EXPECT--
ERROR: [pool unconfined] cannot get uid for user 'aaaaaa'
ERROR: FPM initialization failed
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
