--TEST--
Forwarding a signal to the thread that handles it (GH-25541, GH-25549)
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (!PHP_ZTS) die('skip ZTS only');
if (!function_exists('zend_test_signal_forwarding')) die('skip zend_test_signal_forwarding() not available');
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
    'timer_keeps_no_disposition',
    'handler_outranks_timer',
    'fork',
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
timer_keeps_no_disposition: bool(true)
handler_outranks_timer: bool(true)
fork: bool(true)
