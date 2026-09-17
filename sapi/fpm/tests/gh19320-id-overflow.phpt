--TEST--
FPM: Reject out-of-range numeric user and group IDs
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

require_once "tester.inc";

$id = '18446744073709551615';
$settings = [
    "user = $id",
    "user = 1\ngroup = $id",
    "user = 1\nlisten.owner = $id",
    "user = 1\nlisten.group = $id",
];

foreach ($settings as $setting) {
    $cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR:UDS}}
$setting
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

    $tester = new FPM\Tester($cfg);
    $tester->testConfig();
}

?>
Done
--EXPECT--
ERROR: [pool unconfined] user ID '18446744073709551615' is out of range
ERROR: FPM initialization failed
ERROR: [pool unconfined] group ID '18446744073709551615' is out of range
ERROR: FPM initialization failed
ERROR: [pool unconfined] user ID '18446744073709551615' is out of range
ERROR: FPM initialization failed
ERROR: [pool unconfined] group ID '18446744073709551615' is out of range
ERROR: FPM initialization failed
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
