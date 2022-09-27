--TEST--
FPM: Set arrays in configuration
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
pid = {{FILE:PID}}
[unconfined]
listen = {{ADDR}}
access.suppress_path[] = /ping
access.suppress_path[] = /health_check.php
pm = static
pm.max_children = 5
php_value[error_reporting] = E_ALL
php_value[date.timezone] = Europe/London
php_admin_value[disable_functions] = eval
php_flag[display_errors] = On
php_admin_flag[log_errors] = 1
EOT;

$tester = new FPM\Tester($cfg);
$tester->start(['-tt']);
$tester->expectLogConfigOptions([
    'access.suppress_path[] = /ping',
    'access.suppress_path[] = /health_check.php',
    'php_value[error_reporting] = 32767',
    'php_value[date.timezone] = Europe/London',
    'php_value[display_errors] = 1',
    'php_admin_value[disable_functions] = eval',
    'php_admin_value[log_errors] = 1',
]);


?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
