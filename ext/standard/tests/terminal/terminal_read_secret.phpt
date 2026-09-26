--TEST--
Io\Terminal\Terminal: readSecret parameter count and non-tty failure handling
--FILE--
<?php

use Io\Terminal\Terminal;

$fp = fopen('php://temp', 'r+');
$nonTty = Terminal::fromStreams($fp);

// Non-tty stream throws RuntimeException
try {
    $nonTty->readSecret();
    echo "FAIL: readSecret succeeded on non-tty\n";
} catch (RuntimeException $e) {
    echo "RuntimeException: " . $e->getMessage() . "\n";
}

// readSecret takes 0 arguments
try {
    $nonTty->readSecret('prompt');
    echo "FAIL: readSecret accepted arguments\n";
} catch (ArgumentCountError $e) {
    echo "ArgumentCountError: " . $e->getMessage() . "\n";
}

?>
--EXPECTF--
RuntimeException: Unable to read secret from terminal
ArgumentCountError: Io\Terminal\Terminal::readSecret() expects exactly 0 arguments, 1 given
