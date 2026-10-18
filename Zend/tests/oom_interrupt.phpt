--TEST--
OOM: zend_interrupt_function
--EXTENSIONS--
pcntl
posix
--FILE--
<?php

// Handle signals from a zend_interrupt_function
pcntl_async_signals(true);

$handled = 0;

pcntl_signal(SIGTERM, function () use (&$handled) {
    echo "Handled SIGTERM\n";
    if (++$handled >= 10) {
        exit;
    }
});

$a = [];
for ($i = 0; ; $i++) {
    $a[0] = $i;
    posix_kill(posix_getpid(), SIGTERM);
}

?>
--EXPECT--
Handled SIGTERM
Handled SIGTERM
Handled SIGTERM
Handled SIGTERM
Handled SIGTERM
Handled SIGTERM
Handled SIGTERM
Handled SIGTERM
Handled SIGTERM
Handled SIGTERM
