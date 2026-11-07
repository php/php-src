--TEST--
FPM: master survives a worker killed with unread output (select)
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
pid = {{FILE:PID}}
events.mechanism = select
[unconfined]
listen = {{ADDR}}
ping.path = /ping
ping.response = pong
pm = static
pm.max_children = 1
catch_workers_output = yes
EOT;

$code = <<<'EOT'
<?php
file_put_contents('php://stderr', "unread output\n");
echo getmypid();
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
// Keep the master stopped so the worker output and the pipe EOF are both
// pending when it wakes up.
$tester->signal('STOP');
$childPid = (int) $tester->request()->getBody();
if ($childPid <= 0) {
    $tester->signal('CONT');
    $tester->terminate();
    $tester->close();
    die("ERROR: no worker PID in the response\n");
}
$tester->signal('KILL', $childPid);
for ($i = 0; $i < 100 && !str_starts_with((string) exec("ps -o stat= -p $childPid 2>/dev/null"), 'Z'); $i++) {
    usleep(10000);
}
$tester->signal('CONT');
$tester->expectLogLine('unread output');
$tester->ping('{{ADDR}}');
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
