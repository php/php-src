--TEST--
FPM: GH-19989 - Access log going to fcgi error stream
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$src = <<<EOT
<?php
echo "OK";
EOT;

$cfg = <<<EOT
[global]
error_log = {{RFILE:LOG:ERR}}
pid = {{RFILE:PID}}
[unconfined]
listen = {{ADDR}}
access.log = {{RFILE:LOG:ACC}}
access.format = "'%m %r%Q%q' %s"
pm = static
pm.max_children = 2
EOT;

$prefix = __DIR__;
$tester = new FPM\Tester($cfg, $src);
$tester->start(['--prefix', $prefix]);
$tester->expectLogStartNotices();
$response = $tester->request()->expectBody('OK');
$response->expectNoError();
$tester->expectAccessLog("'GET /gh19989-access-log-fcgi-stderr.src.php' 200");
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();
$tester->checkAccessLog();

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
