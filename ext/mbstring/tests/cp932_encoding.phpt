--TEST--
Exhaustive test of CP932 encoding verification and conversion
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
srand(4321); /* Make results consistent */
include('encoding_tests.inc');
mb_substitute_character(0x25); // '%'

/* Read in the table of all characters in CP932 */
$validChars = array(); /* CP932 string -> UTF-16BE string */
$fp = fopen(realpath(__DIR__ . '/../docs/CP932.txt'), 'r+');
while ($line = fgets($fp, 256)) {
	if ($line[0] == '#')
		continue;

	if (sscanf($line, "0x%x\t0x%x", $bytes, $codepoint) == 2) {
		if ($bytes < 256)
			$validChars[chr($bytes)] = pack('n', $codepoint);
		else
			$validChars[pack('n', $bytes)] = pack('n', $codepoint);
	}
}

/* Aside from the characters in that table, we also support a 'user' area
 * from 0xF040-0xF9FC, which map to Unicode 'private' codepoints 0xE000-E757 */
$codepoint = 0xE000;
for ($i = 0xF0; $i <= 0xF9; $i++) {
	for ($j = 0x40; $j <= 0xFC; $j++) {
		if ($j == 0x7F)
			continue;
		$validChars[chr($i) . chr($j)] = pack('n', $codepoint);
		$codepoint++;
	}
}

$invalidChars = array();
for ($i = 0; $i < 256; $i++) {
	$byte = chr($i);
	if (($i >= 0x81 && $i <= 0x9F) || ($i >= 0xE0 && $i <= 0xFC)) {
		// Could be first byte of a double-byte character
		for ($j = 0; $j < 256; $j++) {
			$str = $byte . chr($j);
			if (!isset($validChars[$str]))
				$invalidChars[$str] = true;
		}
	} else {
		if (!isset($validChars[$byte]))
			$invalidChars[$byte] = true;
	}
}

/* There are 396 Unicode codepoints which are non-invertible in CP932
 * (multiple CP932 byte sequences map to the same codepoint)
 * Some of these are 3-way pile-ups. I wonder what the fine folks at MicroSoft
 * were thinking when they designed this text encoding. */

/* Everything from 0xED00-0xEEFF falls in this unfortunate category
 * (Other sequences in 0xFA00-0xFBFF map to the same codepoints, and when
 * converting from Unicode back to CP932, we favor the F's rather than the E's) */
$nonInvertible = array();
for ($i = 0xED00; $i <= 0xEEFF; $i++) {
	$bytes = pack('n', $i);
	if (isset($validChars[$bytes])) {
		$nonInvertible[$bytes] = $validChars[$bytes];
		unset($validChars[$bytes]); // will test these separately
	}
}

/* There are 23 other collisions between 2-byte sequences which variously
 * start with 0x81, 0x87, or 0xFA
 * We _love_ 0x81 and use it when possible. 0x87 is a second favorite */
for ($i = 0xFA4A; $i <= 0xFA53; $i++) {
	$bytes = pack('n', $i);
	$nonInvertible[$bytes] = $validChars[$bytes];
	unset($validChars[$bytes]); // will test these separately
}
foreach ([0x8790, 0x8791, 0x8792, 0x8795, 0x8796, 0x8797, 0x879A, 0x879B, 0x879C, 0xFA54, 0xFA58, 0xFA59, 0xFA5A, 0xFA5B] as $i) {
	$bytes = pack('n', $i);
	$nonInvertible[$bytes] = $validChars[$bytes];
	unset($validChars[$bytes]); // will test these separately
}

testAllValidChars($validChars, 'CP932', 'UTF-16BE');
foreach ($nonInvertible as $cp932 => $unicode)
	testValidString($cp932, $unicode, 'CP932', 'UTF-16BE', false);
echo "CP932 verification and conversion works on all valid characters\n";

testAllInvalidChars($invalidChars, $validChars, 'CP932', 'UTF-16BE', "\x00%");
echo "CP932 verification and conversion works on all invalid characters\n";

?>
--EXPECT--
CP932 verification and conversion works on all valid characters
CP932 verification and conversion works on all invalid characters
