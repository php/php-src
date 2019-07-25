--TEST--
Concurrent reload signals should not kill PHP-FPM master process. (Bug: #74083)
--SKIPIF--
<?php
include "skipif.inc";
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
pid = {{FILE:PID}}
process_control_timeout=1
[unconfined]
listen = {{ADDR}}
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 1
EOT;

$code = <<<EOT
<?php
/* empty */
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->ping('{{ADDR}}');

/* Vary interval between concurrent reload requests
    since performance of test instance is not known in advance */
$max_interval = 25000;
$step = 1000;
$pid = $tester->getPid();
for ($interval = 0; $interval < $max_interval; $interval += $step) {
    exec("kill -USR2 $pid", $out, $killExitCode);
    if ($killExitCode) {
        echo "ERROR: master process is dead\n";
        break;
    }
    usleep($interval);
}
echo "Reached interval $interval us with $step us steps\n";
$tester->expectLogNotice('Reloading in progress ...');
/* Consume mix of 'Reloading in progress ...' and 'reloading: .*' */
$tester->getLogLines(2000);

$tester->signal('USR2');
$tester->expectLogNotice('Reloading in progress ...');
$tester->expectLogNotice('reloading: .*');
$tester->expectLogNotice('using inherited socket fd=\d+, "127.0.0.1:\d+"');
$tester->expectLogStartNotices();
$tester->ping('{{ADDR}}');

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECT--
Reached interval 25000 us with 1000 us steps
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
