--TEST--
FPM: Status pool is inheriting FPM setting overrides test - expose_php = on
--SKIPIF--
<?php include "skipif.inc"; ?>
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
pm.status_listen = {{ADDR[status]}}
pm.status_path = /status
php_flag[expose_php] = On
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogStartNotices();
$response = $tester->request('', [], '/status', '{{ADDR[status]}}');
$response->expectHeader('X-Powered-By', '|^PHP/8|', true);
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
