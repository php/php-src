--TEST--
FPM: Output of 4 GiB or more is not truncated
--SKIPIF--
<?php
include "skipif.inc";
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
?>
--INI--
memory_limit=-1
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = 1
php_admin_value[memory_limit] = -1
php_admin_value[output_buffering] = 0
EOT;

$code = <<<EOT
<?php
echo str_repeat('a', 4294967306);
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$body = $tester->request()->getBody();
var_dump(strlen($body));
unset($body);
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECT--
int(4294967306)
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
