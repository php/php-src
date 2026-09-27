--TEST--
Io\Terminal\Terminal: create, fromStreams, construction and serialization restrictions
--FILE--
<?php

use Io\Terminal\Terminal;
use Io\Terminal\TerminalSize;
use Io\Terminal\ModeToken;

// Named constructors
$t1 = Terminal::create();
var_dump($t1 instanceof Terminal);

$fp = fopen('php://temp', 'r+');
$t2 = Terminal::fromStreams($fp);
var_dump($t2 instanceof Terminal);

$fpOut = fopen('php://temp', 'r+');
$t3 = Terminal::fromStreams($fp, $fpOut);
var_dump($t3 instanceof Terminal);

// Invalid stream arguments
try {
    Terminal::fromStreams('invalid');
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

try {
    Terminal::fromStreams($fp, 123);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

// Constructor privacy
foreach ([Terminal::class, TerminalSize::class, ModeToken::class] as $class) {
    try {
        new $class();
    } catch (Throwable $e) {
        echo $e::class, ": ", $e->getMessage(), PHP_EOL;
    }
}

// Clone rejection
foreach ([$t1, $t2] as $obj) {
    try {
        clone $obj;
    } catch (Throwable $e) {
        echo $e::class, ": ", $e->getMessage(), PHP_EOL;
    }
}

// Serialization rejection
try {
    serialize($t1);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
TypeError: Io\Terminal\Terminal::fromStreams(): Argument #1 ($input) must be of type resource, string given
TypeError: Io\Terminal\Terminal::fromStreams(): Argument #2 ($output) must be of type resource or null, int given
Error: %s
Error: %s
Error: %s
Error: Trying to clone an uncloneable object of class Io\Terminal\Terminal
Error: Trying to clone an uncloneable object of class Io\Terminal\Terminal
Exception: Serialization of 'Io\Terminal\Terminal' is not allowed
