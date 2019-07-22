--TEST--
FPM: bug77185 - Reload robustness
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$workers = 10;
$loops   = 10;

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
pid = {{FILE:PID}}
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = $workers
catch_workers_output = true
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogStartNotices();
for ($i = 0; $i < $loops; $i++) {
	$tester->signal('USR2');
	$tester->expectLogNotice('Reloading in progress ...');
	$tester->expectLogNotice('reloading: .*');
	$tester->expectLogNotice('using inherited socket fd=\d+, "127.0.0.1:\d+"');
	$tester->expectLogStartNotices();
}
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
