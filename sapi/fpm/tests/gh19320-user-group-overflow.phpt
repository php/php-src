--TEST--
FPM: gh19320 - config test UID/GID overflow validation
--SKIPIF--
<?php
include "skipif.inc";
FPM\Tester::skipIfNotRoot();
?>
--FILE--
<?php
require_once "tester.inc";

// Test with UID that exceeds INT_MAX (2147483647)
$cfg_uid = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR:UDS}}
user = 2147483648
group = root
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg_uid);
$tester->start();
$tester->expectLogError("\[pool unconfined\] invalid user ID '2147483648': value too large");

// Test with GID that exceeds INT_MAX
$cfg_gid = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR:UDS}}
user = root
group = 4294967295
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg_gid);
$tester->start();
$tester->expectLogError("\[pool unconfined\] invalid group ID '4294967295': value too large");
?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>

