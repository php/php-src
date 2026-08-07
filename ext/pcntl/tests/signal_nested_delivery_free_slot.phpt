--TEST--
At least one signal can be queued during dispatch
--DESCRIPTION--
pcntl_signal_dispatch() dequeues a signal before dispatching it, so at least one
signal is deliverable during dispatch.
--EXTENSIONS--
pcntl
posix
--FILE--
<?php

$indent = '';
$seen_sigusr1 = 0;

pcntl_signal(SIGUSR1, function ($signo) use (&$indent, &$seen_sigusr1) {
    switch (++$seen_sigusr1) {
        case 1:
            echo "{$indent}Handling SIGUSR1\n";
            $indent .= ' ';
            echo "{$indent}Dispatch\n";
            // There is at least one free queue slot, so this is deliverable
            posix_kill(posix_getpid(), SIGUSR2);
            pcntl_signal_dispatch();
            $indent = substr($indent, 0, -1);
            echo "{$indent}Done handling SIGUSR1\n";
            break;
        case 2:
            echo "{$indent}More SIGUSR1...\n";
            break;
        default:
            break;
    }
});

pcntl_signal(SIGUSR2, function ($signo) use (&$indent) {
    echo "{$indent}Handling SIGUSR2\n";
    echo "{$indent}Done handling SIGUSR2\n";
});

// Fill the queue
for ($i = 0; $i < 1024; $i++) {
    posix_kill(posix_getpid(), SIGUSR1);
}
pcntl_signal_dispatch();

?>
--EXPECT--
Handling SIGUSR1
 Dispatch
 More SIGUSR1...
 Handling SIGUSR2
 Done handling SIGUSR2
Done handling SIGUSR1
