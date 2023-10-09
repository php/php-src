--TEST--
Test of ASCII and JIS X 0201/0208/0212 support in ISO-2022-JP and JIS7/8 encodings
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

/* Read in table of all characters in JISX-0212 charset */
readConversionTable(__DIR__ . '/data/JISX0212.txt', $jisx0212Chars, $unused);

/* Read in table of all characters in JISX-0208 charset */
$jisx0208Chars = array(); /* JISX0208 -> UTF-16BE */
$fp = fopen(__DIR__ . '/data/JISX0208.txt', 'r+');
while ($line = fgets($fp, 256)) {
	if ($line[0] == '#')
		continue;

	if (sscanf($line, "0x%x\t0x%x\t0x%x", $shiftJIS, $jis0208Code, $unicodeCP) == 3) {
		$jisx0208Chars[pack('n', $jis0208Code)] = pack('n', $unicodeCP);
	}
}

/* Read in table of all characters in JISX-0201 charset */
readConversionTable(__DIR__ . '/data/JISX0201.txt', $jisx0201Chars, $unused);

/* The JIS X 0208 character set does not have a single, straightforward
 * mapping to the Unicode character set
 * mbstring converts one character differently from the mappings in
 * ../docs/JISX0208.txt, which comes from the Unicode Consortium */

/* 0x2140 is a backslash; this can be mapped to 0x005C for an ordinary
 * backslash, or 0xFF3C for a _fullwidth_ one */
$jisx0208Chars["\x21\x40"] = "\xFF\x3C";

function testValid($from, $to, $encoding, $bothWays = true) {
	identifyValidString($from, $encoding);
	convertValidString($from, $to, $encoding, 'UTF-16BE', false);

	if ($bothWays) {
		/* An 0xF at the beginning of a JIS7 string is redundant; it switches
		 * to ASCII mode, but ASCII mode is default */
		if ($from[0] == "\x0F")
			$from = substr($from, 1, strlen($from) - 1);
		/* ESC ( B at the beginning is redundant, since ASCII mode is the default */
		if (substr($from, 0, 3) == "\x1B(B")
			$from = substr($from, 3, strlen($from) - 3);
		convertValidString($to, $from, 'UTF-16BE', $encoding, false);
	}
}

function testInvalid($from, $to, $encoding) {
	testInvalidString($from, $to, $encoding, 'UTF-16BE');
}

for ($i = 0; $i < 0x80; $i++) {
	if ($i == 0xE || $i == 0xF || $i == 0x1B)
		continue;
	testValid(chr($i),                   "\x00" . chr($i), 'JIS');
	convertValidString("\x0F" . chr($i), "\x00" . chr($i), 'JIS', 'UTF-16BE', false); /* 0xF is 'Shift In' code */
	testValid("\x1B(B" . chr($i),        "\x00" . chr($i), 'JIS');
	testValid(chr($i),                   "\x00" . chr($i), 'ISO-2022-JP');
	testValid("\x1B(B" . chr($i),        "\x00" . chr($i), 'ISO-2022-JP');
}

for ($i = 0x80; $i < 256; $i++) {
	if ($i >= 0xA1 && $i <= 0xDF) // We convert single bytes from 0xA1-0xDF as JIS X 0201 kana
		continue;
	testInvalid(chr($i),            "\x00%", 'JIS');
	testInvalid("\x0F" . chr($i),   "\x00%", 'JIS');
	testInvalid("\x1B(B" . chr($i), "\x00%", 'JIS');
	testInvalid(chr($i),            "\x00%", 'ISO-2022-JP');
	testInvalid("\x1B(B" . chr($i), "\x00%", 'ISO-2022-JP');
}

echo "ASCII support OK\n";

/* All valid JIS X 0201 characters
 * Those with a 1 in the high bit are JIS X 0201 kana; JIS7 encodes those
 * with a 0 in the high bit and treats them as a separate charset
 * (We don't test ISO-2022-JP here, as it does not support the JIS X 0201 charset) */
foreach ($jisx0201Chars as $jisx0201 => $utf16BE) {
	if (ord($jisx0201) >= 128) {
		$kana = chr(ord($jisx0201) - 128);
		testValid("\x1B(I" . $kana . "\x1B(B", $utf16BE, 'JIS', false);
		testValid("\x0E" . $kana . "\x0F", $utf16BE, 'JIS', false); /* 0xE is 'Shift Out' code */
		testValid($jisx0201, $utf16BE, 'JIS', false);
	} else {
		testValid("\x1B(J" . $jisx0201 . "\x1B(B", $utf16BE, 'JIS', $utf16BE > "\x00\x80");
	}
}

for ($i = 0x80; $i < 256; $i++) {
	if ($i >= 0xA1 && $i <= 0xDF)
		continue;
	testInvalid("\x1B(I" . chr($i)  . "\x1B(B", "\x00%", 'JIS');
	testInvalid("\x1B(J" . chr($i)  . "\x1B(B", "\x00%", 'JIS');
}

echo "JIS X 0201 support OK\n";

/* All valid JISX0208 characters */
foreach ($jisx0208Chars as $jisx0208 => $utf16BE) {
	testValid("\x1B\$B" . $jisx0208 . "\x1B(B", $utf16BE, 'JIS');
	testValid("\x1B\$B" . $jisx0208 . "\x1B(B", $utf16BE, 'ISO-2022-JP');
}

/* All invalid 2-byte JISX0208 characters */
for ($i = 0x21; $i <= 0x7E; $i++) {
	for ($j = 0; $j < 256; $j++) {
		$testString = chr($i) . chr($j);
		if (!isset($jisx0208Chars[$testString])) {
			testInvalid("\x1B\$B" . $testString . "\x1B(B", "\x00%", 'JIS');
			testInvalid("\x1B\$B" . $testString . "\x1B(B", "\x00%", 'ISO-2022-JP');
		}
	}
}

/* Try truncated JISX0208 characters */
for ($i = 0x21; $i <= 0x7E; $i++) {
	testInvalid("\x1B\$B" . chr($i), "\x00%", 'JIS');
	testInvalid("\x1B\$B" . chr($i), "\x00%", 'ISO-2022-JP');
}

/* Switch from Kanji to ASCII */
testValidString("\x30\x00\x00A", "\x1B\$B\x21\x21\x1B(BA", "UTF-16BE", "JIS", false);
testValidString("\x30\x00\x00A", "\x1B\$B\x21\x21\x1B(BA", "UTF-16BE", "ISO-2022-JP", false);

echo "JIS X 0208 support OK\n";

/* JIS7 supports escape to switch to JIS X 0212 charset, but ISO-2022-JP does not */

/* All valid JISX0212 characters */
foreach ($jisx0212Chars as $jisx0212 => $utf16BE) {
	testValid("\x1B\$(D" . $jisx0212 . "\x1B(B", $utf16BE, 'JIS', false);
}

/* All invalid 2-byte JISX0212 characters */
for ($i = 0x21; $i <= 0x7E; $i++) {
	for ($j = 0; $j < 256; $j++) {
		$testString = chr($i) . chr($j);
		if (!isset($jisx0212Chars[$testString])) {
			testInvalid("\x1B\$(D" . $testString . "\x1B(B", "\x00%", 'JIS');
		}
	}
}

/* Try truncated JISX0212 characters */
for ($i = 0x21; $i <= 0x7E; $i++) {
	testInvalid("\x1B\$(D" . chr($i) . "\x1B(B", "\x00%\x00%", 'JIS');
}

testValidString("\x00\xA1", "\x1B\$(D\x22\x42\x1B(B", "UTF-16BE", "JIS", false);
// Check that ISO-2022-JP treats JISX 0212 chars as error
convertInvalidString("\x00\xA1", "%", "UTF-16BE", "ISO-2022-JP", false);

echo "JIS X 0212 support OK\n";

/* All possible escape sequences */
$validJisEscapes = ["\x1B\$@" => true, "\x1B\$B" => true, "\x1B\$(@" => true, "\x1B\$(B" => true, "\x1B\$(D" => true, "\x1B(B" => true, "\x1B(H" => true, "\x1B(J" => true, "\x1B(I" => true];
$validIso2022jpEscapes = ["\x1B\$@" => true, "\x1B\$B" => true, "\x1B(B" => true, "\x1B(J" => true];
for ($i = 0; $i <= 0xFF; $i++) {
	for ($j = 0; $j <= 0xFF; $j++) {
		$escapeSequence = "\x1B" . chr($i) . chr($j);
		if ($escapeSequence === "\x1B\$(")
			continue;
		if (isset($validJisEscapes[$escapeSequence])) {
			testValid($escapeSequence . "\x1B(B", "", 'JIS', false);
		} else {
			identifyInvalidString($escapeSequence . "\x1B(B", 'JIS');
		}
		if (isset($validIso2022jpEscapes[$escapeSequence])) {
			testValid($escapeSequence . "\x1B(B", "", 'ISO-2022-JP', false);
		} else {
			identifyInvalidString($escapeSequence . "\x1B(B", 'ISO-2022-JP');
		}
	}
}
for ($i = 0; $i <= 0xFF; $i++) {
	$escapeSequence = "\x1B\$(" . chr($i);
	if (isset($validJisEscapes[$escapeSequence])) {
		testValid($escapeSequence . "\x1B(B", "", 'JIS', false);
	} else {
		identifyInvalidString($escapeSequence . "\x1B(B", 'JIS');
	}
	if (isset($validIso2022jpEscapes[$escapeSequence])) {
		testValid($escapeSequence . "\x1B(B", "", 'ISO-2022-JP', false);
	} else {
		identifyInvalidString($escapeSequence . "\x1B(B", 'ISO-2022-JP');
	}
}
/* Also try a bare ESC */
identifyInvalidString("\x1B", 'JIS');
identifyInvalidString("\x1B", 'ISO-2022-JP');

convertInvalidString("\x1B$", "%", "JIS", "UTF-8");
convertInvalidString("\x1B$", "%", "ISO-2022-JP", "UTF-8");
convertInvalidString("\x1B(", "%", "JIS", "UTF-8");
convertInvalidString("\x1B(", "%", "ISO-2022-JP", "UTF-8");
convertInvalidString("\x1B,", "%,", "JIS", "UTF-8");
convertInvalidString("\x1B,", "%,", "ISO-2022-JP", "UTF-8");

echo "All escape sequences work as expected\n";

foreach (['JIS', 'ISO-2022-JP'] as $encoding) {
	testValidString("\x22\x25", "\x1B\$B!B\x1B(B", 'UTF-16BE', $encoding, false);
	testValidString("\xFF\x0D", "\x1B\$B!]\x1B(B", 'UTF-16BE', $encoding, false);
	testValidString("\xFF\xE0", "\x1B\$B!q\x1B(B", 'UTF-16BE', $encoding, false);
	testValidString("\xFF\xE1", "\x1B\$B!r\x1B(B", 'UTF-16BE', $encoding, false);
	testValidString("\xFF\xE2", "\x1B\$B\"L\x1B(B", 'UTF-16BE', $encoding, false);

	testValidString("\x00\xA5", "\x1B(J\x5C\x1B(B", 'UTF-16BE', $encoding, false);
}
testValidString("\x20\x3E", "\x1B\$B!1\x1B(B", 'UTF-16BE', 'ISO-2022-JP', false);

echo "Other mappings from Unicode -> ISO-2022-JP are OK\n";

// Single bytes from 0xA3-0xDF can be used to encode kana in JIS8
$grInvoked = [
	"\xA3" => "\x1B(I\x23\x1B(B",
	"\xB1" => "\x1B(I\x31\x1B(B",
	"\xC2" => "\x1B(I\x42\x1B(B",
	"\xDF" => "\x1B(I\x5F\x1B(B"
];
foreach ($grInvoked as $gr => $jisx) {
	// JISX 0201 is used as the canonical form for outputting kana
	testValidString($gr, $jisx, 'JIS', 'JIS', false);
	if (mb_convert_encoding($gr, 'UTF-16BE', 'JIS') !== mb_convert_encoding($jisx, 'UTF-16BE', 'JIS'))
		die("Equivalent GR byte and JISX 0201 sequence do not decode to the same codepoint");
}

echo "GR-invoked kana support OK\n";

// Check handling of BOM
convertInvalidString("\xFF\xFE", "%", "UTF-16BE", "JIS", false);
convertInvalidString("\xFF\xFE", "%", "UTF-16BE", "ISO-2022-JP", false);

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\xE0", "%", "JIS", "UTF-8");
convertInvalidString("\xE0", "%", "ISO-2022-JP", "UTF-8");
convertInvalidString("\x1B\$(X", "%\$(X", "JIS", "UTF-8"); // Invalid escape
convertInvalidString("\x1B\$(X", "%\$(X", "ISO-2022-JP", "UTF-8"); // Invalid escape
convertInvalidString("\x1B\$B!", "%", "JIS", "UTF-8"); // Truncated character
convertInvalidString("\x1B\$B!", "%", "ISO-2022-JP", "UTF-8"); // Truncated character

echo "Done!\n";
?>
--EXPECT--
ASCII support OK
JIS X 0201 support OK
JIS X 0208 support OK
JIS X 0212 support OK
All escape sequences work as expected
Other mappings from Unicode -> ISO-2022-JP are OK
GR-invoked kana support OK
Done!
