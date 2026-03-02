--TEST--
Exhaustive test of EUC-JP-MS (AKA EUC-JP-WIN) text encoding
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(555); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

readConversionTable(__DIR__ . '/data/EUC-JP-MS.txt', $toUnicode, $fromUnicode);
readConversionTable(__DIR__ . '/data/EUC-JP-MS.IRREVERSIBLE.txt', $toUnicodeIrreversible, $_);

foreach ($toUnicodeIrreversible as $char => $codepoint) {
  if (!isset($fromUnicode[$codepoint]))
    $fromUnicode[$codepoint] = $char;
}

// The conversion table has several cases where more than one EUC-JP-WIN code maps to the
// same Unicode codepoint. Make sure we use the intended reverse mapping for tests:
$fromUnicode["\x21\x21"] = "\xAD\xE4";
$fromUnicode["\x21\x61"] = "\xAD\xB6";
$fromUnicode["\x21\x62"] = "\xAD\xB7";
$fromUnicode["\x21\x63"] = "\xAD\xB8";
$fromUnicode["\x21\x65"] = "\xAD\xBA";
$fromUnicode["\x21\x68"] = "\xAD\xBD";
$fromUnicode["\x21\x69"] = "\xAD\xBE";

$fromUnicode["\x22\x1A"] = "\xA2\xE5";
$fromUnicode["\x22\x20"] = "\xA2\xDC";
$fromUnicode["\x22\x29"] = "\xA2\xC1";
$fromUnicode["\x22\x2A"] = "\xA2\xC0";
$fromUnicode["\x22\x2B"] = "\xA2\xE9";
$fromUnicode["\x22\x35"] = "\xA2\xE8";
$fromUnicode["\x22\x52"] = "\xA2\xE2";
$fromUnicode["\x22\x61"] = "\xA2\xE1";
$fromUnicode["\x22\xA5"] = "\xA2\xDD";

$fromUnicode["\x32\x31"] = "\xAD\xEA";

$fromUnicode["\xFF\x5E"] = "\xA1\xC1";

findInvalidChars($toUnicode, $invalid, $truncated, array_fill_keys(range(0xA1,0xFE), 2) + [0x8F => 3]);
testAllValidChars($toUnicode, 'eucJP-win', 'UTF-16BE', false);
testAllInvalidChars($invalid, $toUnicode, 'eucJP-win', 'UTF-16BE', "\x00%");
testTruncatedChars($truncated, 'eucJP-win', 'UTF-16BE', "\x00%");
echo "Tested eucJP-win -> UTF-16BE\n";

findInvalidChars($fromUnicode, $invalid, $unused, array_fill_keys(range(0,0xFF), 2));
convertAllInvalidChars($invalid, $fromUnicode, 'UTF-16BE', 'eucJP-win', '%');
echo "Tested UTF-16BE -> eucJP-win\n";

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x80", "%", "eucJP-win", "UTF-8");
convertInvalidString("\xFE\xFF", "%", "eucJP-win", "UTF-8");

?>
--EXPECT--
Tested eucJP-win -> UTF-16BE
Tested UTF-16BE -> eucJP-win
