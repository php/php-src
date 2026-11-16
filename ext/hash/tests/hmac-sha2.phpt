--TEST--
Hash: HMAC-SHA2
--FILE--
<?php
function test($algo, $data, $key, $truncate = 0) {
	$result = hash_hmac($algo, $data, $key);
	($truncate > 0) && $result = substr($result, 0, $truncate * 2);
	echo "{$algo}: {$result}\n";
}

// Samples from https://csrc.nist.gov/projects/cryptographic-standards-and-guidelines/example-values
echo "*** NIST samples ***\n";
foreach (["sha224", "sha256", "sha384", "sha512"] as $algo) {
	test($algo, "Sample message for keylen=blocklen", pack("C*", ... range(0x00, $algo[3] === '2' ? 0x3F : 0x7F)));
	test($algo, "Sample message for keylen<blocklen", pack("C*", ... range(0x00, (substr($algo, -3) / 8) - 1)));
	test($algo, "Sample message for keylen=blocklen", pack("C*", ... range(0x00, $algo[3] === '2' ? 0x63 : 0xC7)));
	test($algo, "Sample message for keylen<blocklen, with truncated tag", pack("C*", ... range(0x00, 0x30)));
}

// These are truncated and full outputs aren't available
echo "*** SP 800-224 ipd ***\n";
test("sha224", hex2bin("ea008790f4f4bb4693bd17fd726517be"), hex2bin("e44e3c2837d83501bd5b5403af653dc608a2b217689e"), 20);
test("sha256", hex2bin("063f0b6e8960826cfbe35ebdb01b47ea"), hex2bin("c8d46cbf65271fcc60db02e4d7cc4bd875"), 16);
test("sha384", hex2bin("3933069e5e5a5bb0aab68c3c1f9fcaf7"), hex2bin("d122ea657d8e3d5c5b69c9fe4ab7368d508e500c3ea2e528d346547a72987086c97668b7c139058a3f454144832ff7ff31ffd48f25936e3a"), 10);
test("sha512", hex2bin("ed39a83534d4d989c6b25fa8a563f51c"), hex2bin("f9e2e43a5fbab3e24fec3a76c249688370544ffad051fe904531c3feb66de453df0a24bbd1b3a43c34788732651eba8a"), 10);
test("sha512/224", hex2bin("4407f708fb4eb39882e7fa552474c595"), hex2bin("6036db046aac5778cef2e795a9787347310907d711d0a2bf1d15b1bfa5eb"), 15);
test("sha512/256", hex2bin("7afe75e5d204235a462bb282c648278c"), hex2bin("d3f8bbe410dc40ea2ba2176bd99e0905c8f8ede67fa40a33897f1ce38cba34c3ad4d5207"), 17);

echo "*** RFC 4231 / RFC 4634 / RFC 6234 ***\n";
foreach (["sha224", "sha256", "sha384", "sha512"] as $algo) {
	test($algo, "Hi There", str_repeat("\x0b", 20));
	test($algo, "what do ya want for nothing?", "Jefe");
	test($algo, str_repeat("\xdd", 50), str_repeat("\xaa", 20));
	test($algo, str_repeat("\xcd", 50), pack("C*", ... range(0x01, 0x19)));
	test($algo, "Test With Truncation", str_repeat("\x0c", 20), 16);
	test($algo, "Test Using Larger Than Block-Size Key - Hash Key First", str_repeat("\xaa", 131));
	test($algo, "This is a test using a larger than block-size key and a larger than block-size data. The key needs to be hashed before being used by the HMAC algorithm.", str_repeat("\xaa", 131));
}
?>
--EXPECT--
*** NIST samples ***
sha224: c7405e3ae058e8cd30b08b4140248581ed174cb34e1224bcc1efc81b
sha224: e3d249a8cfb67ef8b7a169e9a0a599714a2cecba65999a51beb8fbbe
sha224: 91c52509e5af8531601ae6230099d90bef88aaefb961f4080abc014d
sha224: d522f1df596ca4b4b1c23d27bde067d6153ba9725fd5cde0af4a2a42
sha256: 8bb9a1db9806f20df7f77b82138c7914d174d59e13dc4d0169c9057b133e1d62
sha256: a28cf43130ee696a98f14a37678b56bcfcbdd9e5cf69717fecf5480f0ebdf790
sha256: bdccb6c72ddeadb500ae768386cb38cc41c63dbb0878ddb9c7a38a431b78378d
sha256: 27a8b157839efeac98df070b331d593618ddb985d403c0c786d23b5d132e57c7
sha384: 63c5daa5e651847ca897c95814ab830bededc7d25e83eef9195cd45857a37f448947858f5af50cc2b1b730ddf29671a9
sha384: 6eb242bdbb582ca17bebfa481b1e23211464d2b7f8c20b9ff2201637b93646af5ae9ac316e98db45d9cae773675eeed0
sha384: 5b664436df69b0ca22551231a3f0a3d5b4f97991713cfa84bff4d0792eff96c27dccbbb6f79b65d548b40e8564cef594
sha384: c48130d3df703dd7cdaa56800dfbd2ba2458320e6e1f98fec8ad9f57f43800df3615ceb19ab648e1ecdd8c730af95c8a
sha512: fc25e240658ca785b7a811a8d3f7b4ca48cfa26a8a366bf2cd1f836b05fcb024bd36853081811d6cea4216ebad79da1cfcb95ea4586b8a0ce356596a55fb1347
sha512: fd44c18bda0bb0a6ce0e82b031bf2818f6539bd56ec00bdc10a8a2d730b3634de2545d639b0f2cf710d0692c72a1896f1f211c2b922d1a96c392e07e7ea9fedc
sha512: d93ec8d2de1ad2a9957cb9b83f14e76ad6b5e0cce285079a127d3b14bccb7aa7286d4ac0d4ce64215f2bc9e6870b33d97438be4aaa20cda5c5a912b48b8e27f3
sha512: 00f3e9a77bb0f06de15f160603e42b5028758808596664c03e1ab8fb2b0767780563aedc644960d4f0c0c5d239f67a2a61b141e8c871f3d40db2c605588dab92
*** SP 800-224 ipd ***
sha224: 7d832ae46647b47aeee26b65f5f1e51805c78f1e
sha256: 6b800744b38d0a9f2b9d64c582f7d6d9
sha384: 7dd24d9ae7a9d82ea6ca
sha512: 7a047975a81d30e9cf18
sha512/224: f5aa41547f04b336ad6862f64d1f50
sha512/256: 23c7cfbe4921b9a4d862b01b6f86273e24
*** RFC 4231 / RFC 4634 / RFC 6234 ***
sha224: 896fb1128abbdf196832107cd49df33f47b4b1169912ba4f53684b22
sha224: a30e01098bc6dbbf45690f3a7e9e6d0f8bbea2a39e6148008fd05e44
sha224: 7fb3cb3588c6c1f6ffa9694d7d6ad2649365b0c1f65d69d1ec8333ea
sha224: 6c11506874013cac6a2abc1bb382627cec6a90d86efc012de7afec5a
sha224: 0e2aea68a90c8d37c988bcdb9fca6fa8
sha224: 95e9a0db962095adaebe9b2d6f0dbce2d499f112f2d2b7273fa6870e
sha224: 3a854166ac5d9f023f54d517d0b39dbd946770db9c2b95c9f6f565d1
sha256: b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7
sha256: 5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843
sha256: 773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe
sha256: 82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b
sha256: a3b6167473100ee06e0c796c2955552b
sha256: 60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54
sha256: 9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2
sha384: afd03944d84895626b0825f4ab46907f15f9dadbe4101ec682aa034c7cebc59cfaea9ea9076ede7f4af152e8b2fa9cb6
sha384: af45d2e376484031617f78d2b58a6b1b9c7ef464f5a01b47e42ec3736322445e8e2240ca5e69e2c78b3239ecfab21649
sha384: 88062608d3e6ad8a0aa2ace014c8a86f0aa635d947ac9febe83ef4e55966144b2a5ab39dc13814b94e3ab6e101a34f27
sha384: 3e8a69b7783c25851933ab6290af6ca77a9981480850009cc5577c6e1f573b4e6801dd23c4a7d679ccf8a386c674cffb
sha384: 3abf34c3503b2a23a46efc619baef897
sha384: 4ece084485813e9088d2c63a041bc5b44f9ef1012a2b588f3cd11f05033ac4c60c2ef6ab4030fe8296248df163f44952
sha384: 6617178e941f020d351e2f254e8fd32c602420feb0b8fb9adccebb82461e99c5a678cc31e799176d3860e6110c46523e
sha512: 87aa7cdea5ef619d4ff0b4241a1d6cb02379f4e2ce4ec2787ad0b30545e17cdedaa833b7d6b8a702038b274eaea3f4e4be9d914eeb61f1702e696c203a126854
sha512: 164b7a7bfcf819e2e395fbe73b56e0a387bd64222e831fd610270cd7ea2505549758bf75c05a994a6d034f65f8f0e6fdcaeab1a34d4a6b4b636e070a38bce737
sha512: fa73b0089d56a284efb0f0756c890be9b1b5dbdd8ee81a3655f83e33b2279d39bf3e848279a722c806b485a47e67c807b946a337bee8942674278859e13292fb
sha512: b0ba465637458c6990e5a8c5f61d4af7e576d97ff94b872de76f8050361ee3dba91ca5c11aa25eb4d679275cc5788063a5f19741120c4f2de2adebeb10a298dd
sha512: 415fad6271580a531d4179bc891d87a6
sha512: 80b24263c7c1a3ebb71493c1dd7be8b49b46d1f41b4aeec1121b013783f8f3526b56d037e05f2598bd0fd2215d6a1e5295e64f73f63f0aec8b915a985d786598
sha512: e37b6a775dc87dbaa4dfa9f96e5e3ffddebd71f8867289865df5a32d20cdc944b6022cac3c4982b10d5eeb55c3e4de15134676fb6de0446065c97440fa8c6a58
