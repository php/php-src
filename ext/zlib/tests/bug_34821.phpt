--TEST--
Bug #34821 (zlib encoders fail on widely varying binary data)
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php

// test 50 bytes to 50k
$b = array(
	50, 
	500, 
	5000, 
	50000,
//	1000000, // works, but test would take too long
);

$s = '';
$i = 0;

foreach ($b as $size) {
	do {
		$s .= chr(rand(0,255));
	} while (++$i < $size);
	var_dump($s === gzinflate(gzdeflate($s)));
	var_dump($s === gzuncompress(gzcompress($s)));
	var_dump($s === gzinflate(substr(gzencode($s), 10, -8)));
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
