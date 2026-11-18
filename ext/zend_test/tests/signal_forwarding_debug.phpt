--TEST--
Forwarding a signal in states that depend on the timing of the helper thread
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (!PHP_ZTS) die('skip ZTS only');
if (!PHP_DEBUG) die('skip debug build only');
if (!function_exists('zend_test_signal_forwarding')) die('skip zend_test_signal_forwarding() not available');
?>
--INI--
max_execution_time=0
--FILE--
<?php
$scenarios = [
    'pipe_saturation',
    'read_failure_keeps_signal',
    'stale_timer_after_grace',
    'rearmed_timer',
    'exit_during_forwarding',
    'concurrent_unregistration',
    'fork_while_held',
    'helper_restart',
    'idle_retirement',
    'no_retirement_during_grace',
    'closed_stdio',
    'handler_stalled_across_retirement',
    'handler_stalled_across_rearm',
    'fork_with_pending_flag',
    'saturation_storm',
    'timer_keeps_handler',
    'timer_arm_disposition',
    'register_during_thread_exit',
    'helper_start_failure',
    'init_failure',
];
foreach ($scenarios as $scenario) {
    echo $scenario, ': ';
    var_dump(zend_test_signal_forwarding($scenario));
}
?>
--EXPECT--
pipe_saturation: bool(true)
read_failure_keeps_signal: bool(true)
stale_timer_after_grace: bool(true)
rearmed_timer: bool(true)
exit_during_forwarding: bool(true)
concurrent_unregistration: bool(true)
fork_while_held: bool(true)
helper_restart: bool(true)
idle_retirement: bool(true)
no_retirement_during_grace: bool(true)
closed_stdio: bool(true)
handler_stalled_across_retirement: bool(true)
handler_stalled_across_rearm: bool(true)
fork_with_pending_flag: bool(true)
saturation_storm: bool(true)
timer_keeps_handler: bool(true)
timer_arm_disposition: bool(true)
register_during_thread_exit: bool(true)
helper_start_failure: bool(true)
init_failure: bool(true)
