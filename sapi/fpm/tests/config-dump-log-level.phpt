--TEST--
FPM: -tt dumps config as NOTICEs regardless of log level
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
log_level = warning
pid = {{FILE:PID}}
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = 5
php_admin_flag[log_errors] = 1
EOT;

$tester = new FPM\Tester($cfg);
$tester->start(['-tt']);
$tester->expectLogConfigOptions([
    'log_level = WARNING',
    'php_admin_value[log_errors] = 1',
]);
// $tester->expectLogNotice('configuration file %s test is successful', null, 1, true, true);

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
