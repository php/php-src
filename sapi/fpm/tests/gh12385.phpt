--TEST--
FPM: GH-12385 - flush with fastcgi does not force headers to be sent
--SKIPIF--
<?php
include "skipif.inc";
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
EOT;

$code = <<<PHP
<?php
header("X-Test: 12345");
flush();
var_dump(headers_sent());
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$response = $tester->request();
$response->expectHeader("X-Test", "12345");
$response->expectBody("bool(true)");
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
