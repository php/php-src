--TEST--
Bug #28220 (mb_strwidth() returns wrong width values for some Hangul characters)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
$coderange = array(
	range(0x0000, 0x1fff),
	range(0xff60, 0xff9f)
);


foreach ($coderange as $r) {
	$ng = 0;
	foreach ($r as $c) {
		if (mb_strwidth(pack('N1', $c), 'UCS-4BE') != 2) {
			$ng++;
		}
	}
	echo "$ng\n";
}
?>
--EXPECT--
8085
63
