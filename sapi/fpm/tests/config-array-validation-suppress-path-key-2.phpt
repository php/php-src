--TEST--
FPM: Validates arrays in configuration are correctly set - access.suppress_path doesn't accept key with forward slash
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
pid = {{FILE:PID}}
[unconfined]
listen = {{ADDR}}
access.suppress_path[/] = test
pm = static
pm.max_children = 5
EOT;

$tester = new FPM\Tester($cfg);
$tester->start(['-tt']);
$tester->expectLogError("\[%s:%d\] Keys provided to field 'access.suppress_path' are ignored");

?>
Done
--EXPECT--

Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
