--TEST--
Io\Terminal\Terminal: /dev/tty controlling terminal identity
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die("skip /dev/tty test is POSIX-only");
}
$fp1 = @fopen('/dev/tty', 'r+');
$fp2 = @fopen('/dev/tty', 'r+');
if ($fp1 === false || $fp2 === false || !stream_isatty($fp1) || !stream_isatty($fp2)) {
    if ($fp1 !== false) {
        fclose($fp1);
    }
    if ($fp2 !== false) {
        fclose($fp2);
    }
    die("skip /dev/tty controlling terminal not available or not a TTY");
}
fclose($fp1);
fclose($fp2);
?>
--FILE--
<?php

use Io\Terminal\Terminal;
use Io\Terminal\ModeToken;

$ttyFp1 = fopen('/dev/tty', 'r+');
$ttyFp2 = fopen('/dev/tty', 'r+');

$tTty1 = Terminal::fromStreams($ttyFp1);
$tTty2 = Terminal::fromStreams($ttyFp2);

$mTty = $tTty1->enableRawMode();
var_dump($mTty instanceof ModeToken);

// Descriptors to the same controlling terminal share identity
$tTty2->restoreMode($mTty);
echo "restored via tTty2
";

// Mode was already restored; reusing the token throws ValueError
try {
    $tTty1->restoreMode($mTty);
    echo "FAIL: consumed token was accepted
";
} catch (ValueError $e) {
    echo "Caught: ", $e->getMessage(), "
";
}

fclose($ttyFp2);
fclose($ttyFp1);
?>
--EXPECT--
bool(true)
restored via tTty2
Caught: Io\Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token belonging to this terminal
