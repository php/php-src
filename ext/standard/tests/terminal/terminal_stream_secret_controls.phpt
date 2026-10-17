--TEST--
Io\Terminal\Terminal: stream readSecret controls, cancellation, navigation, and UTF-8 backspace
--SKIPIF--
<?php
if (!class_exists(\Io\Terminal\Terminal::class)) die('skip Io\Terminal\Terminal not available');
if (!class_exists(\Time\Duration::class)) die('skip Time\Duration not available');
?>
--FILE--
<?php

use Io\Terminal\Terminal;
use Io\Terminal\TerminalException;
use Time\Duration;

function makeStreamTerminal(string $input): Terminal {
    $fp = fopen('php://memory', 'r+');
    fwrite($fp, $input);
    rewind($fp);
    return Terminal::fromStreams($fp);
}

// 1. Ctrl+C followed by newline: must cancel, not return secret with 0x03
try {
    $t = makeStreamTerminal("\x03\n");
    $sec = $t->readSecret();
    echo "FAIL: Ctrl+C returned secret: " . bin2hex($sec) . "\n";
} catch (TerminalException $e) {
    echo "PASS: Ctrl+C cancelled\n";
}

// 2. Ctrl+D followed by newline: must cancel, not return secret with 0x04
try {
    $t = makeStreamTerminal("\x04\n");
    $sec = $t->readSecret();
    echo "FAIL: Ctrl+D returned secret: " . bin2hex($sec) . "\n";
} catch (TerminalException $e) {
    echo "PASS: Ctrl+D cancelled\n";
}

// 3. Escape followed by newline: must cancel, not return secret with 0x1B
try {
    $t = makeStreamTerminal("\x1b\n");
    $sec = $t->readSecret();
    echo "FAIL: Escape returned secret: " . bin2hex($sec) . "\n";
} catch (TerminalException $e) {
    echo "PASS: Escape cancelled\n";
}

// 4. Navigation sequence \x1b[A followed by text must ignore navigation and return text
$t = makeStreamTerminal("\x1b[Aabc\n");
$sec = $t->readSecret();
var_dump($sec === "abc");

// 5. UTF-8 backspace with chr(8) removes 3-byte Euro symbol
$t = makeStreamTerminal("€" . chr(8) . "b\n");
$sec = $t->readSecret();
var_dump($sec === "b");

// 6. ASCII backspace with 0x7F removes preceding byte
$t = makeStreamTerminal("ab\x7fc\n");
$sec = $t->readSecret();
var_dump($sec === "ac");

// 7. SS3 F1 followed by text must ignore F1 and return text (no 'P' corruption)
$t = makeStreamTerminal("\x1bOPpass\n");
$sec = $t->readSecret();
var_dump($sec === "pass");

// 8. CSI control cancellation: Ctrl+C inside CSI sequence must cancel
try {
    $t = makeStreamTerminal("\x1b[\x03pass\n");
    $sec = $t->readSecret();
    echo "FAIL: CSI Ctrl+C returned secret: " . bin2hex($sec) . "\n";
} catch (TerminalException $e) {
    echo "PASS: CSI Ctrl+C cancelled\n";
}

// 9. SS3 control cancellation: Ctrl+C inside SS3 sequence must cancel
try {
    $t = makeStreamTerminal("\x1bO\x03pass\n");
    $sec = $t->readSecret();
    echo "FAIL: SS3 Ctrl+C returned secret: " . bin2hex($sec) . "\n";
} catch (TerminalException $e) {
    echo "PASS: SS3 Ctrl+C cancelled\n";
}

// 10. Zero timeout standalone Escape followed by newline must cancel
try {
    $t = makeStreamTerminal("\x1b\n");
    $sec = $t->readSecret(Duration::fromSeconds(0));
    echo "FAIL: Zero timeout Escape returned secret: " . bin2hex($sec) . "\n";
} catch (TerminalException $e) {
    echo "PASS: Zero timeout Escape cancelled\n";
}

// 11. Zero timeout navigation sequence \x1b[A followed by text must ignore navigation and return text
$t = makeStreamTerminal("\x1b[Apass\n");
$sec = $t->readSecret(Duration::fromSeconds(0));
var_dump($sec === "pass");

// 12. Zero timeout SS3 F1 followed by text must ignore F1 and return text
$t = makeStreamTerminal("\x1bOPpass\n");
$sec = $t->readSecret(Duration::fromSeconds(0));
var_dump($sec === "pass");

?>
--EXPECT--
PASS: Ctrl+C cancelled
PASS: Ctrl+D cancelled
PASS: Escape cancelled
bool(true)
bool(true)
bool(true)
bool(true)
PASS: CSI Ctrl+C cancelled
PASS: SS3 Ctrl+C cancelled
PASS: Zero timeout Escape cancelled
bool(true)
bool(true)
