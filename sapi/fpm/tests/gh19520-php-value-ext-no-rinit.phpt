--TEST--
FPM: GH-19520 - loading shared ext in FPM config must not run RINIT outside of a request
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
$tester->expectLogPattern('/DL TEST MINIT/');
// RINIT must not run when the extension is loaded during the child init (before any request)
$tester->expectNoLogPattern('/DL TEST RINIT/', checkAllLogs: false, timeoutMicroseconds: 200_000);
$tester->request()->expectBody('bool(true)');
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
