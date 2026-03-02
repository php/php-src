--TEST--
mb_str_split() tests for more text encodings
--EXTENSIONS--
mbstring
--FILE--
<?php
$array = mb_str_split("\x00\x01\x02\x03\x04\x05\x06\x07", 2, "UCS-2BE");
echo "[", bin2hex($array[0]), ", ", bin2hex($array[1]), "]\n";

$str = "test カタカナ 汉字";

echo "== HZ ==\n";
$hz = mb_convert_encoding($str, 'HZ', 'UTF-8');
for ($i = 1; $i <= mb_strlen($str, 'UTF-8'); $i++) {
	$array = mb_str_split($hz, $i, 'HZ');
	echo "[", implode(', ', array_map('bin2hex', $array)), "]\n";
	$converted = mb_convert_encoding(implode($array), 'UTF-8', 'HZ');
	if ($converted !== $str) {
		die("Expected " . $str . "; got " . $converted);
	}
}

$str = "test カタカナ 漢字";

echo "== BIG-5 ==\n";
$big5 = mb_convert_encoding($str, 'BIG-5', 'UTF-8');
for ($i = 1; $i <= mb_strlen($str, 'UTF-8'); $i++) {
	$array = mb_str_split($big5, $i, 'BIG-5');
	echo "[", implode(', ', array_map('bin2hex', $array)), "]\n";
	$converted = mb_convert_encoding(implode($array), 'UTF-8', 'BIG-5');
	if ($converted !== $str) {
		die("Expected " . $str . "; got " . $converted);
	}
}

$str = "test ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏ";

echo "== ISO-8859-1 ==\n";
$iso = mb_convert_encoding($str, 'ISO-8859-1', 'UTF-8');
for ($i = 1; $i <= mb_strlen($str, 'UTF-8'); $i++) {
	$array = mb_str_split($iso, $i, 'ISO-8859-1');
	echo "[", implode(', ', array_map('bin2hex', $array)), "]\n";
	$converted = mb_convert_encoding(implode($array), 'UTF-8', 'ISO-8859-1');
	if ($converted !== $str) {
		die("Expected " . $str . "; got " . $converted);
	}
}

echo "== Regression tests ==\n";

// The old implementation of mb_str_split had a bug due to using char* instead of unsigned char*
// When retrieving a byte with the MSB set, it would sign-extend it to become a negative int
// For example, 0xCA would become 0xFFFFFFCA
$array = mb_str_split("\xCA\xCA\xCA\xCA", 2, "JIS");
echo "[", implode(', ', array_map('bin2hex', $array)), "]\n";

// Another bug in the old implementation of mb_str_split; when finishing one last (incomplete)
// incomplete chunk, it required that the last byte of the input string should decode to
// some codepoint for the last chunk to actually be returned
// This is not always the case; the last byte might be some kind of control sequence which
// affects the decoder state but doesn't actually decode to any codepoint
$array = mb_str_split("Z~", 2, "HZ");
echo "[", implode(', ', array_map('bin2hex', $array)), "]\n";

// Another problem with the old implementation of mb_str_split: If you passed a huge chunk_len
// argument, it would run the PHP interpreter out of memory
try {
	mb_str_split("abc", 1234567890, "UTF-8");
} catch (ValueError $e) {
	echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
[00010203, 04050607]
== HZ ==
[74, 65, 73, 74, 20, 7e7b252b7e7d, 7e7b253f7e7d, 7e7b252b7e7d, 7e7b254a7e7d, 20, 7e7b3a3a7e7d, 7e7b57567e7d]
[7465, 7374, 207e7b252b7e7d, 7e7b253f252b7e7d, 7e7b254a7e7d20, 7e7b3a3a57567e7d]
[746573, 74207e7b252b7e7d, 7e7b253f252b254a7e7d, 207e7b3a3a57567e7d]
[74657374, 207e7b252b253f252b7e7d, 7e7b254a7e7d207e7b3a3a57567e7d]
[7465737420, 7e7b252b253f252b254a7e7d20, 7e7b3a3a57567e7d]
[74657374207e7b252b7e7d, 7e7b253f252b254a7e7d207e7b3a3a57567e7d]
[74657374207e7b252b253f7e7d, 7e7b252b254a7e7d207e7b3a3a57567e7d]
[74657374207e7b252b253f252b7e7d, 7e7b254a7e7d207e7b3a3a57567e7d]
[74657374207e7b252b253f252b254a7e7d, 207e7b3a3a57567e7d]
[74657374207e7b252b253f252b254a7e7d20, 7e7b3a3a57567e7d]
[74657374207e7b252b253f252b254a7e7d207e7b3a3a7e7d, 7e7b57567e7d]
[74657374207e7b252b253f252b254a7e7d207e7b3a3a57567e7d]
== BIG-5 ==
[74, 65, 73, 74, 20, c743, c757, c743, c762, 20, ba7e, a672]
[7465, 7374, 20c743, c757c743, c76220, ba7ea672]
[746573, 7420c743, c757c743c762, 20ba7ea672]
[74657374, 20c743c757c743, c76220ba7ea672]
[7465737420, c743c757c743c76220, ba7ea672]
[7465737420c743, c757c743c76220ba7ea672]
[7465737420c743c757, c743c76220ba7ea672]
[7465737420c743c757c743, c76220ba7ea672]
[7465737420c743c757c743c762, 20ba7ea672]
[7465737420c743c757c743c76220, ba7ea672]
[7465737420c743c757c743c76220ba7e, a672]
[7465737420c743c757c743c76220ba7ea672]
== ISO-8859-1 ==
[74, 65, 73, 74, 20, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd, ce, cf]
[7465, 7374, 20c0, c1c2, c3c4, c5c6, c7c8, c9ca, cbcc, cdce, cf]
[746573, 7420c0, c1c2c3, c4c5c6, c7c8c9, cacbcc, cdcecf]
[74657374, 20c0c1c2, c3c4c5c6, c7c8c9ca, cbcccdce, cf]
[7465737420, c0c1c2c3c4, c5c6c7c8c9, cacbcccdce, cf]
[7465737420c0, c1c2c3c4c5c6, c7c8c9cacbcc, cdcecf]
[7465737420c0c1, c2c3c4c5c6c7c8, c9cacbcccdcecf]
[7465737420c0c1c2, c3c4c5c6c7c8c9ca, cbcccdcecf]
[7465737420c0c1c2c3, c4c5c6c7c8c9cacbcc, cdcecf]
[7465737420c0c1c2c3c4, c5c6c7c8c9cacbcccdce, cf]
[7465737420c0c1c2c3c4c5, c6c7c8c9cacbcccdcecf]
[7465737420c0c1c2c3c4c5c6, c7c8c9cacbcccdcecf]
[7465737420c0c1c2c3c4c5c6c7, c8c9cacbcccdcecf]
[7465737420c0c1c2c3c4c5c6c7c8, c9cacbcccdcecf]
[7465737420c0c1c2c3c4c5c6c7c8c9, cacbcccdcecf]
[7465737420c0c1c2c3c4c5c6c7c8c9ca, cbcccdcecf]
[7465737420c0c1c2c3c4c5c6c7c8c9cacb, cccdcecf]
[7465737420c0c1c2c3c4c5c6c7c8c9cacbcc, cdcecf]
[7465737420c0c1c2c3c4c5c6c7c8c9cacbcccd, cecf]
[7465737420c0c1c2c3c4c5c6c7c8c9cacbcccdce, cf]
[7465737420c0c1c2c3c4c5c6c7c8c9cacbcccdcecf]
== Regression tests ==
[1b28494a4a1b2842, 1b28494a4a1b2842]
[5a]
mb_str_split(): Argument #2 ($length) is too large
