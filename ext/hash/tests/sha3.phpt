--TEST--
Hash: SHA3 algorithms
--FILE--
<?php
// SHAKE not yet implemented
$algos = ["sha3-224", "sha3-256", "sha3-384", "sha3-512"];

// FIPS 202 links to a larger directory with 6 samples for each function,
// but only these two are byte-oriented (or rather with length % 8 == 0).
//	https://csrc.nist.gov/projects/cryptographic-standards-and-guidelines/example-values
$vectors = ["", str_repeat("\xa3", 200)];

echo "*** FIPS 202 ***\n";
foreach ($algos as $algo) {
	foreach ($vectors as $vector) {
		echo "{$algo}: ", hash($algo, $vector), "\n";
	}
}

?>
--EXPECT--
*** FIPS 202 ***
sha3-224: 6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7
sha3-224: 9376816aba503f72f96ce7eb65ac095deee3be4bf9bbc2a1cb7e11e0
sha3-256: a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a
sha3-256: 79f38adec5c20307a98ef76e8324afbfd46cfd81b22e3973c65fa1bd9de31787
sha3-384: 0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004
sha3-384: 1881de2ca7e41ef95dc4732b8f5f002b189cc1e42b74168ed1732649ce1dbcdd76197a31fd55ee989f2d7050dd473e8f
sha3-512: a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26
sha3-512: e76dfad22084a8b1467fcf2ffa58361bec7628edf5f3fdc0e4805dc48caeeca81b7c13c30adf52a3659584739a2df46be589c51ca1a4a8416df6545a1ce8ba00
