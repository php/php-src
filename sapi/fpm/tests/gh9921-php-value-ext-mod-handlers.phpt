--TEST--
FPM: GH-9921 - loading shared ext in FPM config does not register module handlers
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
env[PHP_DL_TEST_MODULE_DEBUG] = 1
php_admin_value[extension] = dl_test
EOT;

$code = <<<EOT
<?php
var_dump(extension_loaded('dl_test'));
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectBody('bool(true)');
$tester->expectLogPattern('/DL TEST MINIT/');
$tester->expectLogPattern('/DL TEST RINIT/');
$tester->expectLogPattern('/DL TEST RSHUTDOWN/');
$tester->request()->expectBody('bool(true)');
$tester->expectLogPattern('/DL TEST RINIT/');
$tester->expectLogPattern('/DL TEST RSHUTDOWN/');
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
