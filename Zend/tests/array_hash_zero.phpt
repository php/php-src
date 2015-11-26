--TEST--
Accept hashes being equal to zero
--FILE--
<?php

$hashes = [
	"\x8e\x1a\x63\x0f\x61" => 32,
	"\xf7\x17\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x6b\x03\x6a\x13\x63\x17\x6b\x1d\x67" => 64,
];

foreach ($hashes as $hash => $bits) {
	var_dump($hashes[$hash], $bits);
}

?>
--EXPECT--
int(32)
int(32)
int(64)
int(64)
