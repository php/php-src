--TEST--
Forwarding a signal in states that depend on the timing of the helper thread
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (!PHP_ZTS) die('skip ZTS only');
if (!PHP_DEBUG) die('skip debug build only');
if (!function_exists('zend_test_signal_forwarding')) die('skip zend_test_signal_forwarding() not available');
if (!zend_test_signal_forwarding('supported')) die('skip signal forwarding is not supported on this platform');
?>
--INI--
max_execution_time=0
--FILE--
<?php
$scenarios = [
    'pipe_saturation',
    'read_failure_keeps_signal',
    'helper_suspended',
    'stale_timer_after_grace',
    'rearmed_timer',
    'exit_during_forwarding',
    'concurrent_unregistration',
    'fork_while_held',
    'helper_restart',
    'closed_stdio',
    'handler_stalled_across_helper_exit',
    'fork_inherited_handler',
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
helper_suspended: bool(true)
stale_timer_after_grace: bool(true)
rearmed_timer: bool(true)
exit_during_forwarding: bool(true)
concurrent_unregistration: bool(true)
fork_while_held: bool(true)
helper_restart: bool(true)
closed_stdio: bool(true)
handler_stalled_across_helper_exit: bool(true)
fork_inherited_handler: bool(true)
handler_stalled_across_rearm: bool(true)
fork_with_pending_flag: bool(true)
saturation_storm: bool(true)
timer_keeps_handler: bool(true)
timer_arm_disposition: bool(true)
register_during_thread_exit: bool(true)
helper_start_failure: bool(true)
init_failure: bool(true)
