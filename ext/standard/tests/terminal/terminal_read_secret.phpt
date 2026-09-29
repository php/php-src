--TEST--
Io\Terminal\Terminal: readSecret parameter validation and non-tty failure handling
--FILE--
<?php

use Io\Terminal\Terminal;
use Io\Terminal\TerminalException;
use Time\Duration;

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

// readSecret rejects invalid argument type
try {
    $nonTty->readSecret('prompt');
    echo "FAIL: readSecret accepted string argument\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

// readSecret rejects extra arguments
try {
    $nonTty->readSecret(null, null);
    echo "FAIL: readSecret accepted multiple arguments\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

// readSecret rejects negative timeout
try {
    $nonTty->readSecret(Duration::fromSeconds(1)->negate());
    echo "FAIL: readSecret accepted negative timeout\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
bool(true)
Io\Terminal\TerminalException: Unable to read secret from terminal
TypeError: Io\Terminal\Terminal::readSecret(): Argument #1 ($timeout) must be of type ?Time\Duration, string given
ArgumentCountError: Io\Terminal\Terminal::readSecret() expects at most 1 argument, 2 given
ValueError: Io\Terminal\Terminal::readSecret(): Argument #1 ($timeout) must not be negative
