--TEST--
sqlite: UDF binary handling functions
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 

$data = array(
	"hello there",
	"this has a \x00 char in the middle",
	"\x01 this has an 0x01 at the start",
	"this has \x01 in the middle"
	);

foreach ($data as $item) {
	$coded = sqlite_udf_encode_binary($item);
	echo bin2hex($coded) . "\n";
	$decoded = sqlite_udf_decode_binary($coded);
	if ($item != $decoded) {
		echo "FAIL! $item decoded is $decoded\n";
	}
}

echo "OK!\n";

?>
--EXPECT--
68656c6c6f207468657265
0101736768721f6760721f601fff1f626760711f686d1f7367641f6c6863636b64
0102ff1e726667711e665f711e5f6c1e2e762e2f1e5f721e7266631e71725f7072
7468697320686173200120696e20746865206d6964646c65
OK!
