--TEST--
FPM: config test propagates INI errors
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
pm = static
pm.max_children = 5
php_admin_value[memory_limit] = 1MB
EOT;

$tester = new FPM\Tester($cfg);
$tester->testConfig();

?>
Done
--EXPECTF--
ERROR: [pool unconfined] Invalid "memory_limit" setting. Invalid quantity "1MB": unknown multiplier "B", interpreting as "1" for backwards compatibility
ERROR: [pool unconfined] Failed to set memory limit to 1 bytes (Current memory usage is %d bytes)
ERROR: FPM initialization failed
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
