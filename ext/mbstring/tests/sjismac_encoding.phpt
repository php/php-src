--TEST--
Exhaustive test of MacJapanese encoding verification and conversion
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(300); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

/* Read in the table of all characters in MacJapanese */
$validChars = array(); /* MacJapanese string -> UTF-32BE string */
$fromUnicode = array(); /* UTF-16BE -> MacJapanese */
$fp = fopen(__DIR__ . '/data/MacJapanese-SJIS.txt', 'r+');
while ($line = fgets($fp, 256)) {
	if ($line[0] == '#')
		continue;

	$cp1 = $cp2 = $cp3 = $cp4 = $cp5 = null;
	if (sscanf($line, "0x%x\t0x%x+0x%x+0x%x+0x%x+0x%x", $bytes, $cp1, $cp2, $cp3, $cp4, $cp5) >= 2) {
		if ($bytes < 256) {
			$macJap = chr($bytes);
		} else {
			$macJap = pack('n', $bytes);
		}

		if ($cp5) {
			$validChars[$macJap] = pack('NNNNN', $cp1, $cp2, $cp3, $cp4, $cp5);
			$fromUnicode[pack('nnnnn', $cp1, $cp2, $cp3, $cp4, $cp5)] = $macJap;
		} else if ($cp4) {
			$validChars[$macJap] = pack('NNNN', $cp1, $cp2, $cp3, $cp4);
			$fromUnicode[pack('nnnn', $cp1, $cp2, $cp3, $cp4)] = $macJap;
		} else if ($cp3) {
			$validChars[$macJap] = pack('NNN', $cp1, $cp2, $cp3);
			$fromUnicode[pack('nnn', $cp1, $cp2, $cp3)] = $macJap;
		} else if ($cp2) {
			$validChars[$macJap] = pack('NN', $cp1, $cp2);
			$fromUnicode[pack('nn', $cp1, $cp2)] = $macJap;
		} else {
			$validChars[$macJap] = pack('N', $cp1);
			$fromUnicode[pack('n', $cp1)] = $macJap;
		}
	}
}
/* Although not included in the table, 0x0-0x1F and 0x7F are valid;
 * these are 'control characters' */
for ($i = 0; $i < 0x20; $i++) {
	$validChars[chr($i)] = pack('N', $i);
	$fromUnicode[pack('n', $i)] = chr($i);
}
$validChars["\x7F"] = pack('N', 0x7F);
$fromUnicode["\x00\x7F"] = "\x7F";

/* While Shift-JIS 0x815C normally corresponds to U+2015 (HORIZONTAL BAR),
 * for MacJapanese we convert 0x815C to U+2014 (EM DASH)
 * (See recommendations in JAPANESE.txt from the Unicode Consortium, under
 * 'Unicode mapping issues', point 3)
 * However, when converting Unicode -> MacJapanese, we accept both U+2014
 * and U+2015 */
$fromUnicode["\x20\x15"] = "\x81\x5C";

/* Convert U+203E (OVERLINE) to 0x8150 (FULLWIDTH MACRON) */
$fromUnicode["\x20\x3E"] = "\x81\x50";
/* And also U+00AF (MACRON) */
$fromUnicode["\x00\xAF"] = "\x81\x50";

/* Convert U+FF5E (FULLWIDTH TILDE) to 0x8160 (WAVE DASH) */
$fromUnicode["\xFF\x5E"] = "\x81\x60";

testAllValidChars($validChars, 'SJIS-mac', 'UTF-32BE');
echo "MacJapanese verification and conversion works on all valid characters\n";

findInvalidChars($validChars, $invalidChars, $truncated,
	array_fill_keys(range(0x81, 0x9F), 2) + array_fill_keys(range(0xE0, 0xED), 2));
testAllInvalidChars($invalidChars, $validChars, 'SJIS-mac', 'UTF-32BE', "\x00\x00\x00%");
testTruncatedChars($truncated, 'SJIS-mac', 'UTF-32BE', "\x00\x00\x00%");
echo "MacJapanese verification and conversion rejects all invalid characters\n";

testAllValidChars($fromUnicode, 'UTF-16BE', 'SJIS-mac', false);
echo "Unicode -> SJIS-mac conversion works on all valid characters\n";

findInvalidChars($fromUnicode, $invalidChars, $unused, array_fill_keys(range(0, 0xFF), 2));
convertAllInvalidChars($invalidChars, $fromUnicode, 'UTF-16BE', 'SJIS-mac', '%');
echo "Unicode -> SJIS-mac conversion works on all invalid characters\n";

// Regression test
convertValidString("\x20\x26\x6B\xAA", "\x81\x63\x9F\x6F", "UTF-16BE", "SJIS-mac");

// Test special combining characters for MacJapanese when *not* appearing in
// an expected combination
convertInvalidString("\x20\x10\xF8\x7A", "\x81\x5D%", "UTF-16BE", "SJIS-mac");
convertInvalidString("\x20\x10\x20\xDD", "\x81\x5D%", "UTF-16BE", "SJIS-mac");
convertInvalidString("\x20\x10\xF8\x7F", "\x81\x5D%", "UTF-16BE", "SJIS-mac");
convertInvalidString("\x21\xE6\xF8\x7E", "\x86\xD0%", "UTF-16BE", "SJIS-mac");

convertInvalidString("\xF8\x60\x00\x30\x12\x34", "%%%", "UTF-16BE", "SJIS-mac");
convertInvalidString("\xF8\x61\x00\x46\x00\x41\x12\x34", "%%%%", "UTF-16BE", "SJIS-mac");
convertInvalidString("\xF8\x62\x00\x58\x00\x49\x00\x49\x12\x34", "%%%%%", "UTF-16BE", "SJIS-mac");

// Test "long" illegal character markers
mb_substitute_character("long");
convertInvalidString("\x81", "%", "SJIS-mac", "UTF-8");
convertInvalidString("\x81\x20", "%", "SJIS-mac", "UTF-8");
convertInvalidString("\xED\x9F", "%", "SJIS-mac", "UTF-8");

echo "Done!\n";
?>
--EXPECT--
MacJapanese verification and conversion works on all valid characters
MacJapanese verification and conversion rejects all invalid characters
Unicode -> SJIS-mac conversion works on all valid characters
Unicode -> SJIS-mac conversion works on all invalid characters
Done!
