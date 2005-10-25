--TEST--
bug 34821
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php

// test 50 bytes to 500k
$b = array(
	50, 
	500, 
	5000, 
	500000,
//	1000000, // works, but test would take too long
);

srand(time());

foreach ($b as $size) {
	$s = '';
	for ($i = 0; $i <= $size; ++$i) {
		$s .= chr(rand(0,255));
	}
	var_dump($s == gzinflate(gzdeflate($s)));
	var_dump($s == gzuncompress(gzcompress($s)));
	var_dump($s == gzinflate(substr(gzencode($s), 10, -8)));
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
