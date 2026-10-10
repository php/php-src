--TEST--
Io\Terminal\Terminal: pending UTF-8 handoff from readKey to readSecret
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

// Helper to stage a stream with \xE2, timeout in readKey, then feed continuation
function stageHandoff(string $continuation): Terminal {
    $fp = fopen('php://memory', 'r+');
    fwrite($fp, "\xE2");
    rewind($fp);
    $term = Terminal::fromStreams($fp);
    $k = $term->readKey(Duration::fromSeconds(0));
    // Verify \xE2 timed out and is retained
    if ($k !== null) {
        echo "Unexpected key: " . bin2hex($k) . "\n";
    }
    fwrite($fp, $continuation);
    fseek($fp, 1);
    return $term;
}

// 1. Valid continuation: \xE2 + \x82\xAC + pass\n => €pass
$t1 = stageHandoff("\x82\xACpass\n");
$s1 = $t1->readSecret();
var_dump($s1 === "€pass");

// 2. Non-continuation Ctrl+C: must trigger cancellation, NOT swallow \x03 into secret
$t2 = stageHandoff("\x03\n");
try {
    $s2 = $t2->readSecret();
    echo "FAIL: Ctrl+C swallowed into secret: " . bin2hex($s2) . "\n";
} catch (TerminalException $e) {
    echo "PASS: Ctrl+C cancelled after incomplete UTF-8\n";
}

// 3. Non-continuation Ctrl+D: must trigger cancellation, NOT swallow \x04 into secret
$t3 = stageHandoff("\x04\n");
try {
    $s3 = $t3->readSecret();
    echo "FAIL: Ctrl+D swallowed into secret: " . bin2hex($s3) . "\n";
} catch (TerminalException $e) {
    echo "PASS: Ctrl+D cancelled after incomplete UTF-8\n";
}

// 4. Non-continuation Escape: must trigger cancellation, NOT swallow \x1B into secret
$t4 = stageHandoff("\x1b\n");
try {
    $s4 = $t4->readSecret();
    echo "FAIL: Escape swallowed into secret: " . bin2hex($s4) . "\n";
} catch (TerminalException $e) {
    echo "PASS: Escape cancelled after incomplete UTF-8\n";
}

// 5. Non-continuation Enter: must terminate secret, NOT swallow newline into secret text
$t5 = stageHandoff("\nsecond\n");
try {
    $s5 = $t5->readSecret();
    // The secret must be terminated by the first newline
    var_dump($s5 !== null && strpos($s5, "\n") === false);
} catch (TerminalException $e) {
    // Or throw if invalid UTF-8 at terminator
    echo "PASS: Enter terminated/threw on incomplete UTF-8\n";
}

// 6. Non-continuation Backspace: must not appear in secret content
$t6 = stageHandoff("\x08pass\n");
$s6 = $t6->readSecret();
var_dump(strpos($s6, "\x08") === false);

?>
--EXPECTF--
bool(true)
PASS: Ctrl+C cancelled after incomplete UTF-8
PASS: Ctrl+D cancelled after incomplete UTF-8
PASS: Escape cancelled after incomplete UTF-8
bool(true)
bool(true)
