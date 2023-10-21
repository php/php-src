--TEST--
FPM: gh12232 - loading shared ext in FPM config
--SKIPIF--
<?php
include "skipif.inc";
FPM\Tester::skipIfSharedExtensionNotFound('dl_test');
?>
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
pm.status_path = /status
catch_workers_output = yes
php_admin_value[extension] = dl_test
EOT;

$code = <<<EOT
<?php
var_dump(extension_loaded('dl_test'));
var_dump(ini_get('dl_test.string'));
ini_set('dl_test.string', 'test');
var_dump(ini_get('dl_test.string'));
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectBody(['bool(true)', 'string(5) "hello"', 'string(4) "test"']);
$tester->request()->expectBody(['bool(true)', 'string(5) "hello"', 'string(4) "test"']);
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
<?php
