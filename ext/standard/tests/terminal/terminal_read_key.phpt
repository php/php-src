--TEST--
Io\Terminal\Terminal: readKey with Time\Duration timeouts and parameter validation
--FILE--
<?php

use Io\Terminal\Terminal;
use Time\Duration;

$t = Terminal::create();

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

// Non-terminal stream returns false immediately
$fp = fopen('php://temp', 'r+');
$nonTty = Terminal::fromStreams($fp);
var_dump($nonTty->readKey(Duration::fromSeconds(0)));

?>
--EXPECT--
ValueError: Io\Terminal\Terminal::readKey(): Argument #1 ($timeout) must not be negative
ValueError: Io\Terminal\Terminal::readKey(): Argument #2 ($sequenceTimeout) must not be negative
bool(false)
