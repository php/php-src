--TEST--
Io\Terminal\Terminal: readSecret parameter count and non-tty failure handling
--FILE--
<?php

use Io\Terminal\Terminal;
use Io\Terminal\TerminalException;

var_dump(is_subclass_of(TerminalException::class, \Io\IoException::class));

$fp = fopen('php://temp', 'r+');
$nonTty = Terminal::fromStreams($fp);

// Non-tty stream throws TerminalException
try {
    $nonTty->readSecret();
    echo "FAIL: readSecret succeeded on non-tty\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

// readSecret takes 0 arguments
try {
    $nonTty->readSecret('prompt');
    echo "FAIL: readSecret accepted arguments\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
bool(true)
Io\Terminal\TerminalException: Unable to read secret from terminal
ArgumentCountError: Io\Terminal\Terminal::readSecret() expects exactly 0 arguments, 1 given
