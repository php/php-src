--TEST--
Test function gztell() by calling it with its expected arguments when writing
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
$f = "gztell_basic2.txt.gz";
$h = gzopen($f, 'w');
$sizes = array(7, 22, 54, 17, 27, 15, 1000);
// tell should be 7, 29, 83, 100, 127, 142, 1142

var_dump(gztell($h));
foreach ($sizes as $size) {
   echo "bytes written=".gzwrite($h, str_repeat('1', $size))."\n";
   echo "tell=".gztell($h)."\n";
}

gzclose($h);
unlink($f);
?>
===DONE===
--EXPECT--
int(0)
bytes written=7
tell=7
bytes written=22
tell=29
bytes written=54
tell=83
bytes written=17
tell=100
bytes written=27
tell=127
bytes written=15
tell=142
bytes written=1000
tell=1142
===DONE===
