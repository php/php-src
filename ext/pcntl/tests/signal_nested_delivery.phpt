--TEST--
Signals can be delivered during handler execution
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
pcntl_signal_dispatch();

?>
--EXPECT--
Handling SIGUSR1
 Handling SIGUSR1
  Handling SIGUSR2
  Done handling SIGUSR2
  Handling SIGUSR2
  Done handling SIGUSR2
 Done handling SIGUSR1
Done handling SIGUSR1
