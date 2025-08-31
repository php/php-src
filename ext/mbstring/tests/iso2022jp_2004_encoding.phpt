--TEST--
Exhaustive test of ISO-2022-JP-2004 encoding verification and conversion
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(111); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

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

/* The JIS X 0208 character set does not have a single, straightforward
 * mapping to the Unicode character set
 * mbstring converts one character differently from the mappings in
 * data/JISX0208.txt, which comes from the Unicode Consortium */

/* 0x2140 is a backslash; this can be mapped to 0x005C for an ordinary
 * backslash, or 0xFF3C for a _fullwidth_ one */
$jisx0208Chars["\x21\x40"] = "\xFF\x3C";

/* Single bytes from 0x0-0x20 are allowed */
for ($i = 0; $i <= 0x20; $i++) {
	if ($i != 0x1B)
		$jisx0208Chars[chr($i)] = "\x00" . chr($i);
}
/* As is 0x7F */
$jisx0208Chars["\x7F"] = "\x00\x7F";

/* Now read table of JISX-0213:2004 plane 1 and JISX-0213:2000 plane 2 chars */
$jisx0213_2004_1Chars = array();
$jisx0213_2000_2Chars = array();
$fp = fopen(__DIR__ . '/data/ISO-2022-JP-2004-JISX0213.txt', 'r+');
while ($line = fgets($fp, 256)) {
	if ($line[0] == '#')
		continue;

	$cp2 = null;
	if (sscanf($line, "%d-%x\tU+%x+%x", $type, $bytes, $cp1, $cp2) >= 3) {
		if ($cp1 <= 0xFFFF)
			$unicode = pack('n', $cp1);
		else
			$unicode = mb_convert_encoding(pack('N', $cp1), 'UTF-16BE', 'UTF-32BE');
		if ($cp2)
			$unicode .= pack('n', $cp2);

		if ($type == 3)
			$jisx0213_2004_1Chars[pack('n', $bytes)] = $unicode;
		else if ($type == 4)
			$jisx0213_2000_2Chars[pack('n', $bytes)] = $unicode;
	}
}

/* JISX 0213 plane 1 0x2131 is an overline; Unicode has a halfwidth overline
 * at 0x203E and a fullwidth overline at 0xFFE3
 * We'll use the fullwidth version when converting JISX 0213 to Unicode */
$jisx0213_2004_1Chars["\x21\x31"] = "\xFF\xE3";
/* Same deal with the Yen sign; use the fullwidth one */
$jisx0213_2004_1Chars["\x21\x6F"] = "\xFF\xE5";

/* Since JISX 0213 is an extension of JISX 0208, allow the same single-byte chars */
for ($i = 0; $i <= 0x20; $i++) {
	if ($i != 0x1B)
		$jisx0213_2004_1Chars[chr($i)] = "\x00" . chr($i);
}
$jisx0213_2004_1Chars["\x7F"] = "\x00\x7F";

for ($i = 0; $i <= 0x20; $i++) {
	if ($i != 0x1B)
		$jisx0213_2000_2Chars[chr($i)] = "\x00" . chr($i);
}
$jisx0213_2000_2Chars["\x7F"] = "\x00\x7F";

function testValid($from, $to, $bothWays = true) {
	identifyValidString($from, 'ISO-2022-JP-2004');
	convertValidString($from, $to, 'ISO-2022-JP-2004', 'UTF-16BE', false);

	if ($bothWays) {
		/* Try going in the opposite direction too
		 * ESC ( B at the beginning of ISO-2022-JP-2004 string is redundant,
		 * since ASCII mode is the default */
		if (substr($from, 0, 3) == "\x1B(B")
			$from = substr($from, 3, strlen($from) - 3);
		/* If the ISO-2022-JP-2004 string switches to a different charset, it
		 * should switch back to ASCII at the end */
		if (strpos($from, "\x1B\$B") !== false || strpos($from, "\x1B\$(Q") !== false || strpos($from, "\x1B\$(P") !== false)
			$from .= "\x1B(B";

		convertValidString($to, $from, 'UTF-16BE', 'ISO-2022-JP-2004', false);
	}
}

function testInvalid($from, $to) {
	testInvalidString($from, $to, 'ISO-2022-JP-2004', 'UTF-16BE');
}

/* Try all ASCII characters */
for ($i = 0; $i <= 0x7F; $i++) {
	if ($i == 0x1B)
		continue;
	testValid(chr($i), "\x00" . chr($i));
}

/* Try all ASCII characters, with explicit ASCII escape */
for ($i = 0; $i <= 0x7F; $i++) {
	if ($i == 0x1B)
		continue;
	testValid("\x1B(B" . chr($i), "\x00" . chr($i));
}

echo "Encoding verification and conversion works for all ASCII characters\n";

for ($i = 0x80; $i <= 0x9F; $i++) {
	convertInvalidString("\x00" . chr($i), "%", 'UTF-16BE', 'ISO-2022-JP-2004');
}

echo "Codepoints from U+0080-009F are rejected\n";

/* Try a bare ESC */
identifyInvalidString("\x1B", 'ISO-2022-JP-2004');

/* Try all non-ASCII, non-ESC single bytes */
for ($i = 0x80; $i <= 0xFF; $i++) {
	testInvalid(chr($i), "\x00%");
}

echo "Encoding verification and conversion rejects all invalid single bytes\n";

/* All valid JISX0208 characters */
foreach ($jisx0208Chars as $jisx0208 => $utf16BE) {
	/* Since JIS X 0213 charset is a superset of JIS X 0208, we don't bother
	 * using JIS X 0208 when converting Unicode to ISO-2022-JP-2004
	 * Therefore, don't test conversion in both directions here */
	testValid("\x1B\$B" . $jisx0208, $utf16BE, false);
}

/* All invalid 1-byte JISX0208 characters */
for ($i = 0; $i < 256; $i++) {
	if ($i == 0x1B)
		continue;
	if ($i >= 0x21 && $i <= 0x7E)
		continue;
	$testString = chr($i);
	if (!isset($jisx0208Chars[$testString])) {
		testInvalid("\x1B\$B" . $testString, "\x00%");
	}
}

/* All invalid 2-byte JISX0208 characters */
for ($i = 0x21; $i <= 0x7E; $i++) {
	for ($j = 0; $j < 256; $j++) {
		$testString = chr($i) . chr($j);
		if (!isset($jisx0208Chars[$testString])) {
			testInvalid("\x1B\$B" . $testString, "\x00%");
		}
	}
}

echo "Encoding verification and conversion work on JISX-0208 characters\n";

/* All JISX0213 plane 1 characters */
foreach ($jisx0213_2004_1Chars as $jisx0213_2004 => $utf16BE) {
	/* For single bytes, don't try conversion in both directions */
	testValid("\x1B$(Q" . $jisx0213_2004, $utf16BE, $utf16BE > "\x01\x00");
}

/* All invalid 2-byte JISX0213 plane 1 characters */
for ($i = 0x21; $i <= 0x7E; $i++) {
	for ($j = 0; $j < 256; $j++) {
		$testString = chr($i) . chr($j);
		if (!isset($jisx0213_2004_1Chars[$testString])) {
			testInvalid("\x1B$(Q" . $testString, "\x00%");
		}
	}
}

echo "Encoding verification and conversion work on JISX-0213:2004 plane 1 characters\n";

/* All JISX0213 plane 2 characters */
foreach ($jisx0213_2000_2Chars as $jisx0213_2000 => $utf16BE) {
	/* For single bytes, don't try conversion in both directions */
	testValid("\x1B$(P" . $jisx0213_2000, $utf16BE, $utf16BE > "\x01\x00");
}

/* All invalid 2-byte JISX0213 plane 2 characters */
for ($i = 0x21; $i <= 0x7E; $i++) {
	for ($j = 0; $j < 256; $j++) {
		$testString = chr($i) . chr($j);
		if (!isset($jisx0213_2000_2Chars[$testString])) {
			testInvalid("\x1B$(P" . $testString, "\x00%");
		}
	}
}

echo "Encoding verification and conversion work on JISX-0213:2000 plane 2 characters\n";

/* All possible escape sequences */
$validEscapes = ["\x1B\$B" => true, "\x1B(B" => true, "\x1B$(Q" => true, "\x1B$(P" => true];
for ($i = 0; $i <= 0xFF; $i++) {
	for ($j = 0; $j <= 0xFF; $j++) {
		$escapeSequence = "\x1B" . chr($i) . chr($j);
		if (isset($validEscapes[$escapeSequence])) {
			testValid($escapeSequence, "", false);
		} else {
			identifyInvalidString($escapeSequence, 'ISO-2022-JP-2004');
		}
	}
}

echo "All escape sequences work as expected\n";

identifyInvalidString("\x1B$", 'ISO-2022-JP-2004');
identifyInvalidString("\x1B(", 'ISO-2022-JP-2004');
identifyInvalidString("\x1B$(", 'ISO-2022-JP-2004');

echo "All incomplete escape sequences are rejected\n";

/* Try all combinations of 2 different charsets in the same string */
$ascii = "\x1B(Ba";
$jisx0208 = "\x1B\$B" . array_keys($jisx0208Chars)[rand(0,1000)];
$jisx0213_1 = "\x1B$(Q" . array_keys($jisx0213_2004_1Chars)[rand(0,1000)];
$jisx0213_2 = "\x1B$(P" . array_keys($jisx0213_2000_2Chars)[rand(0,1000)];
$differentCharsets = [$ascii, $jisx0208, $jisx0213_1, $jisx0213_2];
foreach ($differentCharsets as $a) {
	foreach ($differentCharsets as $b) {
		identifyValidString($a . $b, 'ISO-2022-JP-2004');
	}
}

/* Try redundant escape sequences (switching mode but including any characters
 * in the new mode) */
$ascii_Esc = "\x1B(B";
$jisx0208_Esc = "\x1B\$B";
$jisx0213_1_Esc = "\x1B$(Q";
$jisx0213_2_Esc = "\x1B$(P";
$differentCharsets = [$ascii_Esc, $jisx0208_Esc, $jisx0213_1_Esc, $jisx0213_2_Esc];
foreach ($differentCharsets as $a) {
	foreach ($differentCharsets as $b) {
		testValid($a . $b, "", false);
	}
}

echo "Combining multiple charsets in the same string works as expected\n";

/* Try ending in the middle of a JISX0208 character */
testInvalid(substr($jisx0208, 0, strlen($jisx0208) - 1), "\x00%");

/* Try ending in the middle of a JISX0213 plane 1 character */
testInvalid(substr($jisx0213_1, 0, strlen($jisx0213_1) - 1), "\x00%");

/* Try ending in the middle of a JISX0213 plane 2 character */
testInvalid(substr($jisx0213_2, 0, strlen($jisx0213_2) - 1), "\x00%");

echo "Strings with truncated multi-byte characters are rejected\n";

/* We have tried converting all kinds of strings with single characters;
 * now try some random examples of strings with multiple characters */
$jisx0208 = array_keys($jisx0208Chars);
shuffle($jisx0208);
$jisx0213_1 = array_keys($jisx0213_2004_1Chars);
shuffle($jisx0213_1);
$jisx0213_2 = array_keys($jisx0213_2000_2Chars);
shuffle($jisx0213_2);

for ($i = 0; $i < 100; $i++) {
	$size = rand(5,20);
	$testString = '';
	$convertsTo = '';

	/* Build a string from a random combination of characters in the supported
	 * character sets */
	while ($size--) {
		$type  = rand(0,4);
		$chars = rand(0,10);
		if ($type == 0) { /* ASCII */
			$testString .= "\x1B(B";
			while ($chars--) {
				$ascii = chr(rand(0x20, 0x7E));
				$testString .= $ascii;
				$convertsTo .= "\x00" . $ascii;
			}
		} else if ($type == 1) { /* JIS X 0208 */
			$testString .= "\x1B\$B";
			while ($chars--) {
				$jis = array_pop($jisx0208);
				$testString .= $jis;
				$convertsTo .= $jisx0208Chars[$jis];
			}
		} else if ($type == 2) { /* JIS X 0213:2004 plane 1 */
			$testString .= "\x1B$(Q";
			while ($chars--) {
				$jis = array_pop($jisx0213_1);
				$testString .= $jis;
				$convertsTo .= $jisx0213_2004_1Chars[$jis];
			}
		} else { /* JIS X 0213:2000 plane 2 */
			$testString .= "\x1B$(P";
			while ($chars-- && !empty($jisx0213_2)) {
				$jis = array_pop($jisx0213_2);
				$testString .= $jis;
				$convertsTo .= $jisx0213_2000_2Chars[$jis];
			}
		}
	}

	testValid($testString, $convertsTo, false);
}

// Regression test: Test handling of 0x80-0x9F; these have a special meaning in EUC-JP-2004,
// but not in ISO-2022-JP-2004
for ($i = 0x80; $i <= 0x9F; $i++)
	convertInvalidString(chr($i), "%", "ISO-2022-JP-2004", "UTF-8");

// Regression test: Codepoint which has a special representation in EUC-JP-2004
convertInvalidString("\xFF\x95", "%", "UTF-16BE", "ISO-2022-JP-2004");

// Regression test: Old implementation did not switch properly between JIS X 0213 plane 1
// and plane 2
// So try a character which is in plane 1 followed by one in plane 2
testValidString("\x30\x00\x4E\x02", "\x1B\$(Q\x21\x21\x1B\$(P\x21\x22\x1B(B", "UTF-16BE", "ISO-2022-JP-2004");
// Try plane 2 followed by plane 1
testValidString("\x4E\x02\x30\x00", "\x1B\$(P\x21\x22\x1B\$(Q\x21\x21\x1B(B", "UTF-16BE", "ISO-2022-JP-2004");

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\xE0", "%", "ISO-2022-JP-2004", "UTF-8");
convertInvalidString("\x1B\$(X", "%", "ISO-2022-JP-2004", "UTF-8"); // Invalid escape
convertInvalidString("\x1B\$B!", "%", "ISO-2022-JP-2004", "UTF-8"); // Truncated character

// Test sequences of 2 Unicode codepoints which convert to a single character in ISO-2022-JP-2004
testValidString("\x02\x54\x03\x00", "\x1B\$(Q+H\x1B(B", "UTF-16BE", "ISO-2022-JP-2004");
// Including the case where such a codepoint is followed by one which it can't combine with
testValidString("\x02\x54\x00A", "\x1B\$(Q+8\x1B(BA", "UTF-16BE", "ISO-2022-JP-2004");

echo "All done!\n";

?>
--EXPECT--
Encoding verification and conversion works for all ASCII characters
Codepoints from U+0080-009F are rejected
Encoding verification and conversion rejects all invalid single bytes
Encoding verification and conversion work on JISX-0208 characters
Encoding verification and conversion work on JISX-0213:2004 plane 1 characters
Encoding verification and conversion work on JISX-0213:2000 plane 2 characters
All escape sequences work as expected
All incomplete escape sequences are rejected
Combining multiple charsets in the same string works as expected
Strings with truncated multi-byte characters are rejected
All done!
