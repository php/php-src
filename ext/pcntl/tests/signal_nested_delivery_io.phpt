--TEST--
Signals can be delivered during handler execution: I/O
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

$indent = '';

[$read, $write] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
stream_set_blocking($read, true);

pcntl_signal(SIGUSR1, function ($signo) use (&$indent, $read) {
    echo "{$indent}Handling SIGUSR1\n";
    $indent .= ' ';

    // Queue signal
    posix_kill(posix_getpid(), SIGUSR2);
    // Delivered here
    var_dump(fread($read, 1));

    $indent = substr($indent, 0, -1);
    echo "{$indent}Done handling SIGUSR1\n";
});

pcntl_signal(SIGUSR2, function () use (&$indent) {
    echo "{$indent}Handling SIGUSR2\n";
    echo "{$indent}Done handling SIGUSR2\n";
    return Pcntl\SignalReturn::Interrupt;
});

posix_kill(posix_getpid(), SIGUSR1);
pcntl_signal_dispatch();

?>
--EXPECT--
Handling SIGUSR1
 Handling SIGUSR2
 Done handling SIGUSR2
bool(false)
Done handling SIGUSR1
