--TEST--
Signals can be delivered during handler execution
--DESCRIPTION--
pcntl_signal_dispatch() dispatches pending signals if new ones were queued since
the last call (PCNTL_G(pending_signals)=true), including in nested calls.
--EXTENSIONS--
pcntl
posix
--FILE--
<?php

$indent = '';

pcntl_signal(SIGUSR1, function ($signo) use (&$indent) {
    echo "{$indent}Handling SIGUSR1\n";
    $indent .= ' ';
    posix_kill(posix_getpid(), SIGUSR2);
    echo "{$indent}PCNTL_G(pending_signals)=true\n";
    echo "{$indent}Dispatch\n";
    pcntl_signal_dispatch();
    $indent = substr($indent, 0, -1);
    echo "{$indent}Done handling SIGUSR1\n";
});

pcntl_signal(SIGUSR2, function () use (&$indent) {
    echo "{$indent}Handling SIGUSR2\n";
    echo "{$indent}Done handling SIGUSR2\n";
});

posix_kill(posix_getpid(), SIGUSR1);
posix_kill(posix_getpid(), SIGUSR1);
echo "PCNTL_G(pending_signals)=true\n";
echo "Dispatch\n";
pcntl_signal_dispatch();

echo "====\n";

pcntl_signal(SIGUSR1, function ($signo) use (&$indent) {
    echo "{$indent}Handling SIGUSR1\n";
    $indent .= ' ';
    echo "{$indent}Dispatch\n";
    // No-op as no new signal was delivered
    pcntl_signal_dispatch();
    $indent = substr($indent, 0, -1);
    echo "{$indent}Done handling SIGUSR1\n";
});

posix_kill(posix_getpid(), SIGUSR1);
posix_kill(posix_getpid(), SIGUSR1);
echo "PCNTL_G(pending_signals)=true\n";
echo "Dispatch\n";
pcntl_signal_dispatch();

?>
--EXPECT--
PCNTL_G(pending_signals)=true
Dispatch
Handling SIGUSR1
 PCNTL_G(pending_signals)=true
 Dispatch
 Handling SIGUSR1
  PCNTL_G(pending_signals)=true
  Dispatch
  Handling SIGUSR2
  Done handling SIGUSR2
  Handling SIGUSR2
  Done handling SIGUSR2
 Done handling SIGUSR1
Done handling SIGUSR1
====
PCNTL_G(pending_signals)=true
Dispatch
Handling SIGUSR1
 Dispatch
Done handling SIGUSR1
Handling SIGUSR1
 Dispatch
Done handling SIGUSR1
