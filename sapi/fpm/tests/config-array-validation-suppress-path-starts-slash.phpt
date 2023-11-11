--TEST--
FPM: Validates arrays in configuration are correctly set - access.suppress_path begins with forward slash
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
access.suppress_path[] = needs / to start with a slash
pm = static
pm.max_children = 5
EOT;

$tester = new FPM\Tester($cfg);
$tester->start(['-tt']);
$tester->expectLogError("\[%s:%d\] Values provided to field 'access.suppress_path' must begin with '\/'");

?>
Done
--EXPECT--

Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
