--TEST--
FPM: Process restart without any customization
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
pm.max_children = 3
EOT;

$code = <<<EOT
<?php
echo getmypid();
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$pid = $tester->request()->getBody();
if (!is_numeric($pid)) {
    die("ERROR: pid not returned");
}
$tester->signal('TERM', $pid);
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
