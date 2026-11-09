--TEST--
Io\Terminal\Terminal: readSecret parameter validation and empty non-tty handling
--FILE--
<?php

use Io\Terminal\Terminal;
use Io\Terminal\TerminalException;
use Time\Duration;

var_dump(is_subclass_of(TerminalException::class, \Io\IoException::class));

$fp = fopen('php://temp', 'r+');
$nonTty = Terminal::fromStreams($fp);

// Empty non-tty stream without Enter throws TerminalException
try {
    $nonTty->readSecret();
    echo "FAIL: readSecret succeeded on empty non-tty\n";
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
    $nonTty->readSecret(null, null, null);
    echo "FAIL: readSecret accepted extra arguments\n";
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

// readSecret rejects negative sequenceTimeout
try {
    $nonTty->readSecret(null, Duration::fromSeconds(1)->negate());
    echo "FAIL: readSecret accepted negative sequenceTimeout\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

fclose($fp);

?>
--EXPECTF--
bool(true)
Io\Terminal\TerminalException: Unable to read secret from terminal
TypeError: Io\Terminal\Terminal::readSecret(): Argument #1 ($timeout) must be of type ?Time\Duration, string given
ArgumentCountError: Io\Terminal\Terminal::readSecret() expects at most 2 arguments, 3 given
ValueError: Io\Terminal\Terminal::readSecret(): Argument #1 ($timeout) must not be negative
ValueError: Io\Terminal\Terminal::readSecret(): Argument #2 ($sequenceTimeout) must not be negative
