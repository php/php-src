--TEST--
FPM: Output of 4 GiB or more is not truncated
--SKIPIF--
<?php
include "skipif.inc";
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only');
if (!file_exists('/proc/meminfo')) die('skip cannot check free RAM from /proc/meminfo on this platform');
$free_ram = 0;
if ($f = fopen('/proc/meminfo', 'r')) {
    while (!feof($f)) {
        if (preg_match('/MemFree[^\d]*(\d+)/i', fgets($f), $m)) {
            $free_ram = max($free_ram, $m[1] / 1024 / 1024);
        }
    }
    fclose($f);
}
if ($free_ram < 12) die(sprintf('skip need 12G free RAM, but only %01.2f available', $free_ram));
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
