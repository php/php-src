--TEST--
Bug #52981 (Unicode properties are outdated (from Unicode 3.2))
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
function test($str)
{
	$upper = mb_strtoupper($str, 'UTF-8');
	$len = strlen($upper);
	for ($i = 0; $i < $len; ++$i) echo dechex(ord($upper[$i])) . ' ';
	echo "\n";
}

// OK
test("\xF0\x90\x90\xB8");// U+10438 DESERET SMALL LETTER H (added in 3.1.0, March 2001)
// not OK
test("\xE2\xB0\xB0");	// U+2C30 GLAGOLITIC SMALL LETTER AZU (added in 4.1.0, March 2005)
test("\xD4\xA5");		// U+0525 CYRILLIC SMALL LETTER PE WITH DESCENDER (added in 5.2.0, October 2009)
--EXPECT--
f0 90 90 90 
e2 b0 80 
d4 a4 
