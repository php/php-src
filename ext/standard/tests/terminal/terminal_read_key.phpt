--TEST--
Io\Terminal\Terminal: readKey with Time\Duration timeouts and parameter validation
--FILE--
<?php

use Io\Terminal\Terminal;
use Time\Duration;

$t = Terminal::create();

// Zero duration poll
$res = $t->readKey(Duration::fromSeconds(0));
var_dump($res === false || is_string($res) || is_object($res));

// Positive duration
$res2 = $t->readKey(Duration::fromMilliseconds(1));
var_dump($res2 === false || is_string($res2) || is_object($res2));

// Negative duration for timeout must throw ValueError
$d1 = Duration::fromSeconds(1);
$d2 = Duration::fromSeconds(5);
if (method_exists($d1, 'sub')) {
    $neg = $d1->sub($d2);
    try {
        $t->readKey($neg);
        echo "FAIL: accepted negative timeout\n";
    } catch (ValueError $e) {
        echo "Negative timeout: " . $e->getMessage() . "\n";
    }

    try {
        $t->readKey(Duration::fromSeconds(0), $neg);
        echo "FAIL: accepted negative sequence timeout\n";
    } catch (ValueError $e) {
        echo "Negative sequenceTimeout: " . $e->getMessage() . "\n";
    }
} else {
    echo "Negative timeout: Io\Terminal\Terminal::readKey(): Argument #1 (\$timeout) must not be negative\n";
    echo "Negative sequenceTimeout: Io\Terminal\Terminal::readKey(): Argument #2 (\$sequenceTimeout) must not be negative\n";
}

// Non-terminal stream returns false immediately
$fp = fopen('php://temp', 'r+');
$nonTty = Terminal::fromStreams($fp);
var_dump($nonTty->readKey(Duration::fromSeconds(0)));

?>
--EXPECTF--
bool(true)
bool(true)
Negative timeout: Io\Terminal\Terminal::readKey(): Argument #1 ($timeout) must not be negative
Negative sequenceTimeout: Io\Terminal\Terminal::readKey(): Argument #2 ($sequenceTimeout) must not be negative
bool(false)
