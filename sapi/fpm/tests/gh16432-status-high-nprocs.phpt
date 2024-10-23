--TEST--
FPM: GH-16432 - fpm_get_status segfault on high nprocs
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
log_level = notice
[unconfined]
listen = {{ADDR}}
pm = dynamic
pm.max_children = 12800
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 1
catch_workers_output = yes
EOT;

$code = <<<EOT
<?php
var_dump(empty(fpm_get_status()));
EOT;

$tester = new FPM\Tester($cfg, $code);
[$sourceFilePath, $scriptName] = $tester->createSourceFileAndScriptName();
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectBody('bool(false)');
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
