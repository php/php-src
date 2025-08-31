--TEST--
Exhaustive test of verification and conversion of CP950 text
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');
srand(1000); // Make results consistent
mb_substitute_character(0x25); // '%'
readConversionTable(__DIR__ . '/data/CP950.txt', $toUnicode, $fromUnicode);

// Non-reversible mappings (two different CP950 codepoints map to same Unicode codepoint)
$fromUnicode["\x25\x50"] = "\xA2\xA4";
$fromUnicode["\x25\x5E"] = "\xA2\xA5";
$fromUnicode["\x25\x61"] = "\xA2\xA7";
$fromUnicode["\x25\x6A"] = "\xA2\xA6";
$fromUnicode["\x25\x6D"] = "\xA2\x7E";
$fromUnicode["\x25\x6E"] = "\xA2\xA1";
$fromUnicode["\x25\x6F"] = "\xA2\xA3";
$fromUnicode["\x25\x70"] = "\xA2\xA2";

// CP950 explicitly maps some unused Big5 codepoint ranges to Unicode "Private User Area" codepoints

function addPUAMapping($hi, $lo, $unicode) {
  global $fromUnicode, $toUnicode;
  $cp950 = pack('n', ($hi << 8) + $lo);
  $unicode = pack('n', $unicode);
  $fromUnicode[$unicode] = $cp950;
  $toUnicode[$cp950] = $unicode;
}

for ($hi = 0x81; $hi <= 0x8D; $hi++) {
  for ($lo = 0x40; $lo <= 0x7E; $lo++)
    addPUAMapping($hi, $lo, 0xEEB8 + (157 * ($hi - 0x81)) + ($lo - 0x40));
  for ($lo = 0xA1; $lo <= 0xFE; $lo++)
    addPUAMapping($hi, $lo, 0xEEB8 + (157 * ($hi - 0x81)) + ($lo - 0x62));
}

for ($hi = 0x8E; $hi <= 0xA0; $hi++) {
  for ($lo = 0x40; $lo <= 0x7E; $lo++)
    addPUAMapping($hi, $lo, 0xE311 + (157 * ($hi - 0x8E)) + ($lo - 0x40));
  for ($lo = 0xA1; $lo <= 0xFE; $lo++)
    addPUAMapping($hi, $lo, 0xE311 + (157 * ($hi - 0x8E)) + ($lo - 0x62));
}

$hi = 0xC6;
for ($lo = 0xA1; $lo <= 0xFE; $lo++)
  addPUAMapping($hi, $lo, 0xF672 + (157 * ($hi - 0xC6)) + ($lo - 0x62));

for ($hi = 0xC7; $hi <= 0xC8; $hi++) {
  for ($lo = 0x40; $lo <= 0x7E; $lo++)
    addPUAMapping($hi, $lo, 0xF672 + (157 * ($hi - 0xC6)) + ($lo - 0x40));
  for ($lo = 0xA1; $lo <= 0xFE; $lo++)
    addPUAMapping($hi, $lo, 0xF672 + (157 * ($hi - 0xC6)) + ($lo - 0x62));
}

for ($hi = 0xFA; $hi <= 0xFE; $hi++) {
  for ($lo = 0x40; $lo <= 0x7E; $lo++)
    addPUAMapping($hi, $lo, 0xE000 + (157 * ($hi - 0xFA)) + ($lo - 0x40));
  for ($lo = 0xA1; $lo <= 0xFE; $lo++)
    addPUAMapping($hi, $lo, 0xE000 + (157 * ($hi - 0xFA)) + ($lo - 0x62));
}

findInvalidChars($toUnicode, $invalid, $truncated);
testAllValidChars($toUnicode, 'CP950', 'UTF-16BE', false);
testAllInvalidChars($invalid, $toUnicode, 'CP950', 'UTF-16BE', "\x00%");
testTruncatedChars($truncated, 'CP950', 'UTF-16BE', "\x00%");
echo "Tested CP950 -> UTF-16BE\n";

testAllValidChars($fromUnicode, 'UTF-16BE', 'CP950', false);
findInvalidChars($fromUnicode, $invalid, $unused, array_fill_keys(range(0,0xFF), 2));
convertAllInvalidChars($invalid, $fromUnicode, 'UTF-16BE', 'CP950', '%');
echo "Tested UTF-16BE -> CP950\n";

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x80", "%", "CP950", "UTF-8");
convertInvalidString("\x26\x09", "U+2609", "UTF-16BE", "CP950");

echo "Done!\n";
?>
--EXPECT--
Tested CP950 -> UTF-16BE
Tested UTF-16BE -> CP950
Done!
