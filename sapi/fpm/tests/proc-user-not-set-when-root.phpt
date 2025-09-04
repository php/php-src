--TEST--
FPM: Process user setting unset when running as root
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die("skip not for Windows");
}
require_once "tester.inc";

if (!FPM\Tester::findExecutable()) {
    die("skip php-fpm binary not found");
}

FPM\Tester::skipIfNotRoot();
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogAlert(
    "'user' directive has not been specified when running as a root without --allow-to-run-as-root",
    'unconfined'
);
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
