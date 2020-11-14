--TEST--
Exhaustive test of SJIS-2004 encoding verification and conversion
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(101); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

/* Read in the table of all characters in SJIS-2004 */
$validChars = array(); /* SJIS-2004 string -> UTF-32BE string */
$fromUnicode = array(); /* UTF-16BE -> SJIS-2004 */
$fp = fopen(__DIR__ . '/data/SJIS-2004.txt', 'r+');
while ($line = fgets($fp, 256)) {
	if ($line[0] == '#')
		continue;

	$codepoint2 = null;
	if (sscanf($line, "0x%x\tU+%x+%x", $bytes, $codepoint1, $codepoint2) >= 2) {
		$sjis = ($bytes < 256) ? chr($bytes) : pack('n', $bytes);
		if ($codepoint2) {
			$validChars[$sjis] = pack('NN', $codepoint1, $codepoint2);
		} else {
			/* Two input byte sequences can translate to either a 'halfwidth' or a
			 * 'fullwidth' version of a character; our implementation of SJIS-2004
			 * translates them to the fullwidth versions */
			if (preg_match('/Fullwidth: U\+([0-9A-F]+)/', $line, $match))
				$codepoint1 = hexdec($match[1]);
			$validChars[$sjis] = pack('N', $codepoint1);
			if ($codepoint1 <= 0xFFFF)
				$fromUnicode[pack('n', $codepoint1)] = $sjis;
		}
	}
}

/* U+007E is TILDE, Shift-JIS 0x8160 is WAVE DASH */
$fromUnicode["\x00\x7E"] = "\x81\x60";

/* U+005C is backslash, Shift-JIS 0x815F is REVERSE SOLIDUS
 * (ie. a fancy way to say "backslash") */
$fromUnicode["\x00\x5C"] = "\x81\x5F";

testAllValidChars($validChars, 'SJIS-2004', 'UTF-32BE');
echo "SJIS-2004 verification and conversion works for all valid characters\n";

findInvalidChars($validChars, $invalidChars, $truncated,
	array_fill_keys(range(0x81, 0x9F), 2) + array_fill_keys(range(0xE0, 0xFC), 2));
testAllInvalidChars($invalidChars, $validChars, 'SJIS-2004', 'UTF-32BE', "\x00\x00\x00%");
testTruncatedChars($truncated, 'SJIS-2004', 'UTF-32BE', "\x00\x00\x00%");
echo "SJIS-2004 verification and conversion rejects all invalid characters\n";

testAllValidChars($fromUnicode, 'UTF-16BE', 'SJIS-2004', false);
echo "Unicode -> SJIS-2004 conversion works on all valid characters\n";

findInvalidChars($fromUnicode, $invalidChars, $unused, array_fill_keys(range(0, 0xFF), 2));
convertAllInvalidChars($invalidChars, $fromUnicode, 'UTF-16BE', 'SJIS-2004', '%');
echo "Unicode -> SJIS-2004 conversion works on all invalid characters\n";
?>
--EXPECT--
SJIS-2004 verification and conversion works for all valid characters
SJIS-2004 verification and conversion rejects all invalid characters
Unicode -> SJIS-2004 conversion works on all valid characters
Unicode -> SJIS-2004 conversion works on all invalid characters
