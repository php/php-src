--TEST--
FPM: bug68207 - fastcgi.error_header setting headers after sent
--SKIPIF--
<?php
include "skipif.inc"; ?>
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
EOT;

$code = <<<EOT
<?php
echo 1;
fastcgi_finish_request();
d();
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: ['fastcgi.error_header' => '"HTTP/1.1 550 PHP Error"']);
$tester->expectLogStartNotices();
$tester->request()->expectBody('1');
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->expectNoLogPattern('/Cannot modify header information/');
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
