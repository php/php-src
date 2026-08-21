--TEST--
SignalReturn::Interrupt with pending signal interrupts socket read before poll
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
$pair = @stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
if (!$pair) die("skip stream_socket_pair() not available");
?>
--FILE--
<?php
// Signal handler returns Interrupt; signal is sent to self and is pending
// (not yet dispatched) when fread() is called. The top-of-function check in
// php_sockop_read dispatches pending signals before blocking; since the handler
// returns Interrupt it returns -1 immediately instead of entering poll().
pcntl_signal(SIGUSR1, function (int $signo): Pcntl\SignalReturn {
    echo "handler\n";
    return Pcntl\SignalReturn::Interrupt;
}, restart_syscalls: true); // restart_syscalls=true, but handler overrides

[$read, $write] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
stream_set_blocking($read, true);

// Signal is now pending; the PHP handler has not run yet.
posix_kill(posix_getpid(), SIGUSR1);

// fread triggers zend_signal_interrupt_function at the top of php_sockop_read,
// dispatches the pending signal, and returns false immediately.
$result = fread($read, 1024);
var_dump($result);

fclose($read);
fclose($write);
?>
--EXPECT--
handler
bool(false)
