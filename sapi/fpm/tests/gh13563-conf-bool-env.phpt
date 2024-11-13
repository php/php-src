--TEST--
FPM: GH-13563 - conf boolean environment variables values
--SKIPIF--
<?php
include "skipif.inc";
FPM\Tester::skipIfRoot();
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
log_limit = 1024
log_buffering = \${FPM_TEST_LOG_BUF}
daemonize = \${FPM_TEST_DAEMONIZE}
[unconfined]
listen = {{ADDR}}
process.dumpable = \${FPM_TEST_PROC_DUMP}
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
request_terminate_timeout_track_finished = \${FPM_TEST_REQ_TERM_TRACK_FIN}
catch_workers_output = \${FPM_TEST_CATCH_WRK_OUT}
decorate_workers_output = \${FPM_TEST_DECOR_WRK_OUT}
clear_env = \${FPM_TEST_CLEAR_ENV}
EOT;

$code = <<<EOT
<?php
foreach (getenv() as \$name => \$val) {
    if (str_starts_with(\$name, 'FPM_TEST')) {
        printf("%s: %s\n", \$name, \$val);
    }
}
file_put_contents('php://stderr', str_repeat('a', 20) . "\n");
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start(envVars: [
    'FPM_TEST_LOG_BUF' => 'on',
    'FPM_TEST_DAEMONIZE' => 'false',
    'FPM_TEST_PROC_DUMP' => 'no',
    'FPM_TEST_CATCH_WRK_OUT' => 'yes',
    'FPM_TEST_DECOR_WRK_OUT' => 'true',
    'FPM_TEST_CLEAR_ENV' => 'none',
    'FPM_TEST_REQ_TERM_TRACK_FIN' => '0',
]);
$tester->expectLogStartNotices();
$tester->request()->expectBody([
    'FPM_TEST_LOG_BUF: on',
    'FPM_TEST_DAEMONIZE: false',
    'FPM_TEST_PROC_DUMP: no',
    'FPM_TEST_CATCH_WRK_OUT: yes',
    'FPM_TEST_DECOR_WRK_OUT: true',
    'FPM_TEST_CLEAR_ENV: none',
    'FPM_TEST_REQ_TERM_TRACK_FIN: 0',
]);
$tester->terminate();
$tester->expectLogMessage('a', 1024, 20, true);
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
