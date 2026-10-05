--TEST--
Io\Terminal\Terminal: no-op raw-mode leases on non-terminal streams
--FILE--
<?php

use Io\Terminal\Terminal;
use Io\Terminal\ModeToken;

$fp = fopen('php://temp', 'r+');
$terminal = Terminal::fromStreams($fp);

// Non-terminal stream acquires a valid no-op raw mode lease
$token = $terminal->enableRawMode();
var_dump($token instanceof ModeToken);

// getSize() continues to return null on non-terminal streams
var_dump($terminal->getSize());

// Restoring the token succeeds
$terminal->restoreMode($token);
echo "restored non-tty token
";

// Re-restoring the consumed token throws ValueError
try {
    $terminal->restoreMode($token);
    echo "FAIL: re-restoring consumed non-tty token accepted
";
} catch (ValueError $e) {
    echo "Caught: ", $e->getMessage(), PHP_EOL;
}

fclose($fp);
?>
--EXPECT--
bool(true)
NULL
restored non-tty token
Caught: Io\Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token belonging to this terminal
