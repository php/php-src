--TEST--
Exhaustive test of MacJapanese encoding verification and conversion
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
srand(300); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

/* Read in the table of all characters in MacJapanese */
$validChars = array(); /* MacJapanese string -> UTF-32BE string */
$fp = fopen(realpath(__DIR__ . '/../docs/MacJapanese-SJIS.txt'), 'r+');
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
		} else if ($cp4) {
			$validChars[$macJap] = pack('NNNN', $cp1, $cp2, $cp3, $cp4);
		} else if ($cp3) {
			$validChars[$macJap] = pack('NNN', $cp1, $cp2, $cp3);
		} else if ($cp2) {
			$validChars[$macJap] = pack('NN', $cp1, $cp2);
		} else {
			$validChars[$macJap] = pack('N', $cp1);
		}
	}
}
/* Although not included in the table, 0x0-0x1F and 0x7F are valid;
 * these are 'control characters' */
for ($i = 0; $i < 0x20; $i++) $validChars[chr($i)] = pack('N', $i);
$validChars[chr(0x7F)] = pack('N', 0x7F);

testAllValidChars($validChars, 'SJIS-mac', 'UTF-32BE');
echo "MacJapanese verification and conversion works on all valid characters\n";

findInvalidChars($validChars, $invalidChars, $truncated,
	map(range(0x81, 0x9F), 2, map(range(0xE0, 0xED), 2)));
testAllInvalidChars($invalidChars, $validChars, 'SJIS-mac', 'UTF-32BE', "\x00\x00\x00%");
testTruncatedChars($truncated, 'SJIS-mac', 'UTF-32BE', "\x00\x00\x00%");
echo "MacJapanese verification and conversion rejects all invalid characters\n";

$fromUnicode = array_flip($validChars);
findInvalidChars($fromUnicode, $invalidChars, $unused, map(range(0, 0xFF), 2));
testAllInvalidChars($invalidChars, $fromUnicode, 'UTF-32BE', 'SJIS-mac', '%');
echo "Unicode -> SJIS-mac conversion works on all invalid characters\n";
?>
--EXPECT--
MacJapanese verification and conversion works on all valid characters
MacJapanese verification and conversion rejects all invalid characters
Unicode -> SJIS-mac conversion works on all invalid characters
