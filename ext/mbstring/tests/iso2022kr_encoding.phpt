--TEST--
Test of ASCII and KS X 1001-1992 support in ISO-2022-KR encoding
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

readConversionTable(__DIR__ . '/data/KSX1001.txt', $ksxChars, $unused);

function testValid($from, $to, $bothWays = true) {
  identifyValidString($from, 'ISO-2022-KR');
  convertValidString($from, $to, 'ISO-2022-KR', 'UTF-16BE', false);

  if ($bothWays) {
    /* 0xF at the beginning of an ISO-2022 string is redundant; it switches
     * to ASCII mode, but ASCII mode is default */
    if (strlen($from) > 0 && $from[0] == "\x0F")
      $from = substr($from, 1, strlen($from) - 1);
    /* If the string switches to a different charset, it should switch back to
     * ASCII at the end */
    if (strpos($from, "\x1B\$C") !== false)
      $from .= "\x0F";

    convertValidString($to, $from, 'UTF-16BE', 'ISO-2022-KR', false);
  }
}

function testInvalid($from, $to) {
  testInvalidString($from, $to, 'ISO-2022-KR', 'UTF-16BE');
}

testValid("", "");
echo "Empty string OK\n";

for ($i = 0; $i < 0x80; $i++) {
  if ($i == 0xE || $i == 0xF || $i == 0x1B)
    continue;
  testValid(chr($i),          "\x00" . chr($i));
  testValid("\x0F" . chr($i), "\x00" . chr($i)); /* 0xF is 'Shift In' code */
}

for ($i = 0x80; $i < 256; $i++) {
  testInvalid(chr($i),          "\x00%");
  testInvalid("\x0F" . chr($i), "\x00%");
}

echo "ASCII support OK\n";

foreach ($ksxChars as $ksx => $utf16BE) {
  testValid("\x0E" . $ksx, $utf16BE, false);
  testValid("\x1B$)C\x0E" . $ksx, $utf16BE, false);
  testValid("\x1B$)C\x0E" . $ksx . "\x0F", $utf16BE);
}

findInvalidChars($ksxChars, $invalidKsx, $truncatedKsx);

$badChars = array_keys($invalidKsx);
foreach ($badChars as $badChar) {
  if ($badChar[0] == "\x0E" || $badChar[0] == "\x0F" || $badChar[0] == "\x1B")
    continue;
  testInvalid("\x1B$)C\x0E" . $badChar, "\x00%");
}

$badChars = array_keys($truncatedKsx);
foreach ($badChars as $badChar) {
  testInvalid("\x1B$)C\x0E" . $badChar, "\x00%");
}

echo "KS X 1001 support OK\n";

/* After a valid ESC sequence, we are still in ASCII mode; 'Shift Out' is needed to start KS X 1001 */
testValid("\x1B$)Cabc", "\x00a\x00b\x00c", false);

/* Test invalid and truncated ESC sequences */
testInvalid("\x1B", "\x00%");
testInvalid("\x1B$", "\x00%");
testInvalid("\x1B$)", "\x00%");

for ($i = 0; $i < 256; $i++) {
  if (chr($i) != '$')
    testInvalid("\x1B" . chr($i), "\x00%");
  if (chr($i) != ')')
    testInvalid("\x1B$" . chr($i), "\x00%");
  if (chr($i) != 'C')
    testInvalid("\x1B$)" . chr($i), "\x00%");
}

/* We can switch back and forth between ASCII and KS X 1001 */
testValid("\x0E\x0E\x0F\x0E\x0Fabc", "\x00a\x00b\x00c", false);

echo "Escapes behave as expected\n";

?>
--EXPECT--
Empty string OK
ASCII support OK
KS X 1001 support OK
Escapes behave as expected
