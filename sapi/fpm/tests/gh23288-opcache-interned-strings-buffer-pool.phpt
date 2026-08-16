--TEST--
FPM: GH-23288 - opcache.interned_strings_buffer overridden per pool must not crash on restart
--EXTENSIONS--
opcache
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
php_admin_value[opcache.enable] = 1
php_admin_value[opcache.enable_cli] = 1
php_admin_value[opcache.interned_strings_buffer] = 8
EOT;

$code = <<<EOT
<?php
opcache_reset();
opcache_reset();
echo "ok";
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '1',
    'opcache.enable_cli' => '1',
    'opcache.interned_strings_buffer' => '0',
]);
$tester->expectLogStartNotices();
$tester->request()->expectBody('ok');
$tester->request()->expectBody('ok');
$tester->request()->expectBody('ok');
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
