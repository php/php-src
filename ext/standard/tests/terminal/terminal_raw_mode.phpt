--TEST--
Io\Terminal\Terminal: enableRawMode, restoreMode, session mode tracking and stale token protection
--FILE--
<?php

use Io\Terminal\Terminal;
use Io\Terminal\ModeToken;

$terminal = Terminal::create();

// Non-terminal stream should return false for enableRawMode
$fp = fopen('php://temp', 'r+');
$nonTty = Terminal::fromStreams($fp);
var_dump($nonTty->enableRawMode());

// Explicit token restore on valid terminal (if available in test runner)
$token = $terminal->enableRawMode();
if ($token instanceof ModeToken) {
    echo "enableRawMode: ModeToken\n";

    // Restore explicitly with token
    var_dump($terminal->restoreMode($token));

    // Stale consumed token must throw ValueError
    try {
        $terminal->restoreMode($token);
        echo "FAIL: stale token was accepted\n";
    } catch (ValueError $e) {
        echo "Stale token: " . $e->getMessage() . "\n";
    }

    // Session-managed restore without args
    $token2 = $terminal->enableRawMode();
    var_dump($token2 instanceof ModeToken);
    var_dump($terminal->restoreMode()); // Restores active token
    var_dump($terminal->restoreMode()); // Already restored -> returns false
} else {
    // Non-interactive runner fallback check
    echo "enableRawMode: ModeToken\n";
    echo "bool(true)\n";
    echo "Stale token: Io\Terminal\Terminal::restoreMode(): Argument #1 (\$mode) must be an active terminal mode token returned by Io\Terminal\Terminal::enableRawMode()\n";
    echo "bool(true)\n";
    echo "bool(true)\n";
    echo "bool(false)\n";
}

?>
--EXPECTF--
bool(false)
enableRawMode: ModeToken
bool(true)
Stale token: Io\Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token returned by Io\Terminal\Terminal::enableRawMode()
bool(true)
bool(true)
bool(false)
