--TEST--
Io\Terminal\Terminal: readKey with Time\Duration timeouts and parameter validation
--FILE--
<?php

use Io\Terminal\Terminal;
use Time\Duration;

$t = Terminal::fromStdio();

// Negative duration for timeout must throw ValueError
try {
    $t->readKey(Duration::fromSeconds(1)->negate());
    echo "FAIL: accepted negative timeout
";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

try {
    $t->readKey(Duration::fromSeconds(0), Duration::fromSeconds(1)->negate());
    echo "FAIL: accepted negative sequence timeout
";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

// Non-terminal stream reads keys without throwing
$fp = fopen('php://temp', 'r+');
fwrite($fp, "x");
rewind($fp);
$nonTty = Terminal::fromStreams($fp);
$key = $nonTty->readKey(Duration::fromSeconds(0));
var_dump($key === "x");
fclose($fp);

?>
--EXPECT--
ValueError: Io\Terminal\Terminal::readKey(): Argument #1 ($timeout) must not be negative
ValueError: Io\Terminal\Terminal::readKey(): Argument #2 ($sequenceTimeout) must not be negative
bool(true)
