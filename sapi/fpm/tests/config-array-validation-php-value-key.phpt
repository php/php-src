--TEST--
FPM: Validates arrays in configuration are correctly set - php_value array must be passed a key
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
php_value[] = E_ALL
pm = static
pm.max_children = 5
EOT;

$tester = new FPM\Tester($cfg);
$tester->start(['-tt']);
$tester->expectLogError("\[%s:%d\] You must provide a key for field 'php_value'");

?>
Done
--EXPECT--

Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
