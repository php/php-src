--TEST--
Io\Terminal\SystemTerminal: readKey with Time\Duration timeouts and parameter validation
--FILE--
<?php

use Io\Terminal\SystemTerminal;
use Time\Duration;

$t = SystemTerminal::fromStdio();

// Negative duration for timeout must throw ValueError
try {
    $t->readKey(Duration::fromSeconds(1)->negate());
    echo "FAIL: accepted negative timeout\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

try {
    $t->readKey(Duration::fromSeconds(0), Duration::fromSeconds(1)->negate());
    echo "FAIL: accepted negative sequence timeout\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

// Non-terminal stream throws TerminalException
$fp = fopen('php://temp', 'r+');
$nonTty = SystemTerminal::fromStreams($fp);
try {
    $nonTty->readKey(Duration::fromSeconds(0));
    echo "FAIL: readKey on non-terminal did not throw\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: Io\Terminal\SystemTerminal::readKey(): Argument #1 ($timeout) must not be negative
ValueError: Io\Terminal\SystemTerminal::readKey(): Argument #2 ($sequenceTimeout) must not be negative
Io\Terminal\TerminalException: Failed to read key: input stream is not a terminal
