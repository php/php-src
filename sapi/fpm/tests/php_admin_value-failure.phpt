--TEST--
RC violation on failed php_admin_value
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
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
php_admin_value[precision]=-2
EOT;

$code = <<<EOT
<?php
var_dump(ini_get('precision'));
EOT;

$ini = <<<EOT
precision=14
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->setUserIni($ini);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectBody(['string(2) "14"']);
$tester->terminate();
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
