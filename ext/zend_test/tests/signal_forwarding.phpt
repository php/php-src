--TEST--
Forwarding a signal to the thread that handles it (GH-25541, GH-25549)
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (!PHP_ZTS) die('skip ZTS only');
if (!function_exists('zend_test_signal_forwarding')) die('skip zend_test_signal_forwarding() not available');
if (!zend_test_signal_forwarding('supported')) die('skip signal forwarding is not supported on this platform');
?>
--FILE--
<?php
$scenarios = [
    'unmanaged',
    'successor',
    'most_recent_owner',
    'owner_exit',
    'last_owner_exit',
    'thread_without_handler',
    'concurrent_registration',
    'timer_owner',
    'handler_outranks_timer',
    'stale_timer',
    'repeated_signals',
    'replaced_handler',
    'owner_exit_with_pending_signal',
    'blocked_owner_no_failover',
    'thread_identifier_reuse',
    'fork',
    'fork_during_registration',
    'fork_during_notification',
    'repeated_fork',
    'timer_not_inherited',
];
foreach ($scenarios as $scenario) {
    echo $scenario, ': ';
    var_dump(zend_test_signal_forwarding($scenario));
}
?>
--EXPECT--
unmanaged: bool(true)
successor: bool(true)
most_recent_owner: bool(true)
owner_exit: bool(true)
last_owner_exit: bool(true)
thread_without_handler: bool(true)
concurrent_registration: bool(true)
timer_owner: bool(true)
handler_outranks_timer: bool(true)
stale_timer: bool(true)
repeated_signals: bool(true)
replaced_handler: bool(true)
owner_exit_with_pending_signal: bool(true)
blocked_owner_no_failover: bool(true)
thread_identifier_reuse: bool(true)
fork: bool(true)
fork_during_registration: bool(true)
fork_during_notification: bool(true)
repeated_fork: bool(true)
timer_not_inherited: bool(true)
