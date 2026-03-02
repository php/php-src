--TEST--
Setting opcache.enable via php_admin_value fails gracefully
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
catch_workers_output = yes
php_admin_value[opcache.enable] = On
php_admin_flag[display_errors] = On
php_admin_flag[display_startup_errors] = On
php_admin_flag[log_errors] = On
EOT;

$code = <<<EOT
<?php
var_dump(error_get_last());
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '0',
    'opcache.log_verbosity_level' => '2',
]);
$tester->expectLogStartNotices();
$tester->expectLogPattern("/Zend OPcache can't be temporarily enabled. Are you using php_admin_value\\[opcache.enable\\]=1 in an individual pool's configuration?/");
echo $tester
    ->request()
    ->getBody();
$tester->terminate();
$tester->close();

?>
--EXPECT--
NULL
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
