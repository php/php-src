--TEST--
Io\Terminal\Terminal: getSize, TerminalSize readonly properties and environment fallback
--ENV--
COLUMNS=120
LINES=40
--FILE--
<?php

use Io\Terminal\Terminal;
use Io\Terminal\TerminalSize;

$terminal = Terminal::create();
$size = $terminal->getSize();

var_dump($size instanceof TerminalSize);
var_dump(is_int($size->cols) && $size->cols > 0);
var_dump(is_int($size->rows) && $size->rows > 0);

// Readonly enforcement
try {
    $size->cols = 999;
} catch (Error $e) {
    echo "Readonly cols: " . $e->getMessage() . "\n";
}

try {
    $size->rows = 999;
} catch (Error $e) {
    echo "Readonly rows: " . $e->getMessage() . "\n";
}

// Non-terminal stream returns false without env fallback
$fp = fopen('php://temp', 'r+');
$nonTty = Terminal::fromStreams($fp);

putenv('COLUMNS');
putenv('LINES');
var_dump($nonTty->getSize());

// Env fallback works when set
putenv('COLUMNS=100');
putenv('LINES=30');
$fallbackSize = $nonTty->getSize();
var_dump($fallbackSize instanceof TerminalSize);
echo "{$fallbackSize->cols}x{$fallbackSize->rows}\n";

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
Readonly cols: Cannot modify readonly property Io\Terminal\TerminalSize::$cols
Readonly rows: Cannot modify readonly property Io\Terminal\TerminalSize::$rows
bool(false)
bool(true)
100x30
