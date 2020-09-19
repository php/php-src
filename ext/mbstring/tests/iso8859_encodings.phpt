--TEST--
Exhaustive test of verification and conversion of ISO-8859-X text
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(678); // Make results consistent
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

function testValid($from, $to, $n) {
    testValidString($from, $to, "ISO-8859-$n", 'UTF-16BE');
}
function testInvalid($from, $to, $n) {
    testInvalidString($from, $to, "ISO-8859-$n", 'UTF-16BE');
}

for ($n = 1; $n <= 16; $n++) {
    if ($n == 11 || $n == 12)
        continue;

    $toUnicode = array();
    $fromUnicode = array();

    $fp = fopen(realpath(__DIR__ . "/data/8859-$n.txt"), 'r+');
    while ($line = fgets($fp, 256)) {
        if ($line[0] == '#')
            continue;

        if (sscanf($line, "0x%x\t0x%x", $byte, $codepoint) == 2) {
            $toUnicode[$byte] = pack('n', $codepoint);
            $fromUnicode[$codepoint] = chr($byte);
        }
    }

    // Try all ISO-8859-{$n} bytes (these are single-byte encodings)
    for ($i = 0; $i < 256; $i++) {
        if (isset($toUnicode[$i])) {
            testValid(chr($i), $toUnicode[$i], $n);
        } else {
            testInvalid(chr($i), "\x00%", $n);
        }
    }

    // Try all Unicode codepoints up to 0xFFFF
    for ($i = 0; $i <= 0xFFFF; $i++) {
        if (isset($fromUnicode[$i])) {
            testValid($fromUnicode[$i], pack('n', $i), $n);
        } else if (($i & 0xfc00) != 0xd800) {
            convertInvalidString(pack('n', $i), "%", 'UTF-16BE', "ISO-8859-{$n}");
        }
    }

    echo "All is fine and dandy for ISO-8859-$n text\n";
}

?>
--EXPECT--
All is fine and dandy for ISO-8859-1 text
All is fine and dandy for ISO-8859-2 text
All is fine and dandy for ISO-8859-3 text
All is fine and dandy for ISO-8859-4 text
All is fine and dandy for ISO-8859-5 text
All is fine and dandy for ISO-8859-6 text
All is fine and dandy for ISO-8859-7 text
All is fine and dandy for ISO-8859-8 text
All is fine and dandy for ISO-8859-9 text
All is fine and dandy for ISO-8859-10 text
All is fine and dandy for ISO-8859-13 text
All is fine and dandy for ISO-8859-14 text
All is fine and dandy for ISO-8859-15 text
All is fine and dandy for ISO-8859-16 text
