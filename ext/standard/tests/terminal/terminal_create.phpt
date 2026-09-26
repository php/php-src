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
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    Terminal::fromStreams($fp, 123);
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

// Constructor privacy
foreach ([Terminal::class, TerminalSize::class, ModeToken::class] as $class) {
    try {
        new $class();
    } catch (Error $e) {
        echo "Private constructor: " . $class . "\n";
    }
}

// Clone rejection
foreach ([$t1, $t2] as $obj) {
    try {
        clone $obj;
    } catch (Error $e) {
        echo "Clone rejected: " . $e->getMessage() . "\n";
    }
}

// Serialization rejection
try {
    serialize($t1);
} catch (Exception $e) {
    echo "Serialize rejected: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
Io\Terminal\Terminal::fromStreams(): Argument #1 ($input) must be of type resource, string given
Io\Terminal\Terminal::fromStreams(): Argument #2 ($output) must be of type resource or null, int given
Private constructor: Io\Terminal\Terminal
Private constructor: Io\Terminal\TerminalSize
Private constructor: Io\Terminal\ModeToken
Clone rejected: Trying to clone an uncloneable object of class Io\Terminal\Terminal
Clone rejected: Trying to clone an uncloneable object of class Io\Terminal\Terminal
Serialize rejected: Serialization of 'Io\Terminal\Terminal' is not allowed
