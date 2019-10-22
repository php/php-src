--TEST--
FPM: bug76601 children should not ignore signals during concurrent reloads
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
; some value twice greater than tester->getLogLines() timeout
process_control_timeout=10
[unconfined]
listen = {{ADDR}}
; spawn children immediately after reload
pm = static
pm.max_children = 10
EOT;

$code = <<<EOT
<?php
/* empty */
EOT;

/*
 * If a child miss SIGQUIT then reload process should stuck
 * for at least process_control_timeout that is set greater
 * than timout in log reading functions.
 *
 * Alternative way is to set log_level=debug and filter result of
 * $tester->getLogLines(2000) for lines containing SIGKILL
 * 
 *     [22-Oct-2019 03:28:19.532703] DEBUG: pid 21315, fpm_pctl_kill_all(), line 161: [pool unconfined] sending signal 9 SIGKILL to child 21337
 *     [22-Oct-2019 03:28:19.533471] DEBUG: pid 21315, fpm_children_bury(), line 259: [pool unconfined] child 21337 exited on signal 9 (SIGKILL) after 1.003055 seconds from start
 * 
 * but it has less probability of failure detection. Additionally it requires more
 * $tester->expectLogNotice() around last reload due to presence of debug messages.
 */

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();

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
$skipped = $tester->getLogLines(2000);

$tester->signal('USR2');
$tester->expectLogNotice('Reloading in progress ...');
/* When a child ignores SIGQUIT, the following expectation fails due to timeout. */
if (!$tester->expectLogNotice('reloading: .*')) {
    /* for troubleshooting */
    echo "Skipped messages\n";
    echo implode('', $skipped);
}
$tester->expectLogNotice('using inherited socket fd=\d+, "127.0.0.1:\d+"');
$tester->expectLogStartNotices();

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
