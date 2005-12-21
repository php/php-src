--TEST--
Bug #30549 (incorrect character translations for some ISO8859 charsets)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
test('ISO-8859-7',  array(0xa4 => 0x20ac, 0xa5 => 0x20af, 0xaa => 0x037a));
test('ISO-8859-8',  array(0xaf => 0x00af, 0xfd => 0x200e, 0xfe => 0x200f));
test('ISO-8859-10', array(0xa4 => 0x012a                                ));

function test($enc, $map) {
	print "$enc\n";

	foreach($map as $fromc => $toc) {
		$ustr = mb_convert_encoding(pack('C', $fromc), 'UCS-4BE', $enc);
		foreach (unpack('Nc', $ustr) as $unic);
		printf("0x%04x, 0x%04x\n", $toc, $unic);
	}
}
?>
--EXPECT--
ISO-8859-7
0x20ac, 0x20ac
0x20af, 0x20af
0x037a, 0x037a
ISO-8859-8
0x00af, 0x00af
0x200e, 0x200e
0x200f, 0x200f
ISO-8859-10
0x012a, 0x012a
