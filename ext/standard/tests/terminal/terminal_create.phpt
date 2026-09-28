--TEST--
Io\Terminal\Terminal: fromStdio, fromStreams, construction and serialization restrictions
--FILE--
<?php

use Io\Terminal\Terminal;
use Io\Terminal\TerminalSize;
use Io\Terminal\ModeToken;

// Named constructors
$t1 = Terminal::fromStdio();
var_dump($t1);

$fp = fopen('php://temp', 'r+');
$t2 = Terminal::fromStreams($fp);
var_dump($t2);

$fpOut = fopen('php://temp', 'r+');
$t3 = Terminal::fromStreams($fp, $fpOut);
var_dump($t3);

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

// Constructor privacy for Terminal and ModeToken
foreach ([Terminal::class, ModeToken::class] as $class) {
    try {
        new $class();
    } catch (Throwable $e) {
        echo $e::class, ": ", $e->getMessage(), PHP_EOL;
    }
}

// TerminalSize public constructor and validation
$size = new TerminalSize(80, 24);
var_dump($size->cols, $size->rows);

foreach ([
    [0, 24],
    [80, 0],
    [-1, 24],
    [80, -5],
] as [$cols, $rows]) {
    try {
        new TerminalSize($cols, $rows);
    } catch (Throwable $e) {
        echo $e::class, ": ", $e->getMessage(), PHP_EOL;
    }
}

// TerminalSize serialization
$serialized = serialize($size);
$unserialized = unserialize($serialized);
var_dump($unserialized instanceof TerminalSize, $unserialized->cols, $unserialized->rows);

// Clone rejection
foreach ([$t1, $t2] as $obj) {
    try {
        clone $obj;
    } catch (Throwable $e) {
        echo $e::class, ": ", $e->getMessage(), PHP_EOL;
    }
}

// Serialization rejection for Terminal
try {
    serialize($t1);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
object(Io\Terminal\Terminal)#1 (0) {
}
object(Io\Terminal\Terminal)#2 (0) {
}
object(Io\Terminal\Terminal)#3 (0) {
}
TypeError: Io\Terminal\Terminal::fromStreams(): Argument #1 ($input) must be of type resource, string given
TypeError: Io\Terminal\Terminal::fromStreams(): Argument #2 ($output) must be of type resource or null, int given
Error: Call to private Io\Terminal\Terminal::__construct() from global scope
Error: Call to private Io\Terminal\ModeToken::__construct() from global scope
int(80)
int(24)
ValueError: Io\Terminal\TerminalSize::__construct(): Argument #1 ($cols) must be greater than 0
ValueError: Io\Terminal\TerminalSize::__construct(): Argument #2 ($rows) must be greater than 0
ValueError: Io\Terminal\TerminalSize::__construct(): Argument #1 ($cols) must be greater than 0
ValueError: Io\Terminal\TerminalSize::__construct(): Argument #2 ($rows) must be greater than 0
bool(true)
int(80)
int(24)
Error: Trying to clone an uncloneable object of class Io\Terminal\Terminal
Error: Trying to clone an uncloneable object of class Io\Terminal\Terminal
Exception: Serialization of 'Io\Terminal\Terminal' is not allowed
