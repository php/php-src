--TEST--
Hash: HMAC-RIPEMD
--FILE--
<?php
// https://homes.esat.kuleuven.be/~bosselae/ripemd160.html
$algos   = [
	"ripemd128" => [hex2bin("00112233445566778899aabbccddeeff"),         hex2bin("0123456789abcdeffedcba9876543210")],
	"ripemd160" => [hex2bin("00112233445566778899aabbccddeeff01234567"), hex2bin("0123456789abcdeffedcba987654321000112233")]
];
$vectors = [
	"",
	"a",
	"abc",
	"message digest",
	"abcdefghijklmnopqrstuvwxyz",
	"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
	str_repeat("1234567890", 8),
	str_repeat("a", 1000000)
];

echo "*** RIPEMD reference ***\n";
foreach ($algos as $algo => $keys) {
	foreach ($vectors as $vector) {
		echo "{$algo}[key 1]: ", hash_hmac($algo, $vector, $keys[0]), "\n";
		echo "{$algo}[key 2]: ", hash_hmac($algo, $vector, $keys[1]), "\n";
	}
}

echo "*** RFC 2286 ***\n";
foreach (["ripemd128" => 16, "ripemd160" => 20] as $algo => $keySize) {
	echo "{$algo}: ", hash_hmac($algo, "Hi There", str_repeat("\x0b", $keySize)), "\n";
	echo "{$algo}: ", hash_hmac($algo, "what do ya want for nothing?", "Jefe"), "\n";
	echo "{$algo}: ", hash_hmac($algo, str_repeat("\xdd", 50), str_repeat("\xaa", $keySize)), "\n";
	echo "{$algo}: ", hash_hmac($algo, str_repeat("\xcd", 50), pack("C*", ... range(0x01, 0x19))), "\n";
	echo "{$algo}: ", hash_hmac($algo, "Test With Truncation", str_repeat("\x0c", $keySize)), "\n";
	echo "{$algo}: ", hash_hmac($algo, "Test Using Larger Than Block-Size Key - Hash Key First", str_repeat("\xaa", 80)), "\n";
	echo "{$algo}: ", hash_hmac($algo, "Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data", str_repeat("\xaa", 80)), "\n";
}
?>
--EXPECT--
*** RIPEMD reference ***
ripemd128[key 1]: ad9db2c1e22af9ab5ca9dbe5a86f67dc
ripemd128[key 2]: 8931eeee56a6b257fd1ab5418183d826
ripemd128[key 1]: 3bf448c762de00bcfa0310b11c0bde4c
ripemd128[key 2]: dbbcf169ea7419d5ba7bd8eb3673ff2d
ripemd128[key 1]: f34ec0945f02b70b8603f89e1ce4c78c
ripemd128[key 2]: 2c4cd07d3162d6a0e338004d6b6fbc9a
ripemd128[key 1]: e8503a8aec2289d82aa0d8d445a06bdd
ripemd128[key 2]: 75bfb25888f4bb77c77ae83ad0817447
ripemd128[key 1]: ee880b735ce3126065de1699cc136199
ripemd128[key 2]: b1b5dc0fcb7258758855dd1840fcdce4
ripemd128[key 1]: 794daf2e3bdeea2538638a5ced154434
ripemd128[key 2]: 670d0f7a697b18f1a8ab7d2a2a00dbc1
ripemd128[key 1]: 3a06eef165b23625247800be23e232b6
ripemd128[key 2]: 54e315fdb34a61c0475392e5c7852998
ripemd128[key 1]: 9a4f0159c0952da43a8d466d46b0af58
ripemd128[key 2]: ad04354d8aa2a623e72e3594ee3535c0
ripemd128[key 1]: 19b1b3af333b894dd86d09427116d0ad
ripemd128[key 2]: 6f9b1c0fc06753618d6db4b007733795
ripemd160[key 1]: cf387677bfda8483e63b57e06c3b5ecd8b7fc055
ripemd160[key 2]: fe69a66c7423eea9c8fa2eff8d9dafb4f17a62f5
ripemd160[key 1]: 0d351d71b78e36dbb7391c810a0d2b6240ddbafc
ripemd160[key 2]: 85743e899bc82dbfa36faaa7a25b7cfd372432cd
ripemd160[key 1]: f7ef288cb1bbcc6160d76507e0a3bbf712fb67d6
ripemd160[key 2]: 6e4afd501fa6b4a1823ca3b10bd9aa0ba97ba182
ripemd160[key 1]: f83662cc8d339c227e600fcd636c57d2571b1c34
ripemd160[key 2]: 2e066e624badb76a184c8f90fba053330e650e92
ripemd160[key 1]: 843d1c4eb880ac8ac0c9c95696507957d0155ddb
ripemd160[key 2]: 07e942aa4e3cd7c04dedc1d46e2e8cc4c741b3d9
ripemd160[key 1]: 60f5ef198a2dd5745545c1f0c47aa3fb5776f881
ripemd160[key 2]: b6582318ddcfb67a53a67d676b8ad869aded629a
ripemd160[key 1]: e49c136a9e5627e0681b808a3b97e6a6e661ae79
ripemd160[key 2]: f1be3ee877703140d34f97ea1ab3a07c141333e2
ripemd160[key 1]: 31be3cc98cee37b79b0619e3e1c2be4f1aa56e6c
ripemd160[key 2]: 85f164703e61a63131be7e45958e0794123904f9
ripemd160[key 1]: c2aa88c6405658dc225e485488371fb2433fa735
ripemd160[key 2]: 82a504a002ba6e6c67f3cd67cedb66dc169bab7a
*** RFC 2286 ***
ripemd128: fbf61f9492aa4bbf81c172e84e0734db
ripemd128: 875f828862b6b334b427c55f9f7ff09b
ripemd128: 09f0b2846d2f543da363cbec8d62a38d
ripemd128: bdbbd7cf03e44b5aa60af815be4d2294
ripemd128: e79808f24b25fd031c155f0d551d9a3a
ripemd128: dc732928de98104a1f59d373c150acbb
ripemd128: 5c6bec96793e16d40690c237635f30c5
ripemd160: 24cb4bd67d20fc1a5d2ed7732dcc39377f0a5668
ripemd160: dda6c0213a485a9e24f4742064a7f033b43c4069
ripemd160: b0b105360de759960ab4f35298e116e295d8e7c1
ripemd160: d5ca862f4d21d5e610e18b4cf1beb97a4365ecf4
ripemd160: 7619693978f91d90539ae786500ff3d8e0518e39
ripemd160: 6466ca07ac5eac29e1bd523e5ada7605b791fd8b
ripemd160: 69ea60798d71616cce5fd0871e23754cd75d5a0a
