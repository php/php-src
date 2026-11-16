--TEST--
Hash: sha2 algorithms
--FILE--
<?php
function test($algo, $data) {
	echo "{$algo}: ", hash($algo, $data), "\n";
}

function monteCarloTest($algo, $seed) {
	for ($j = 0; $j < 4; $j++) {
		$md = [$seed, $seed, $seed];
		for ($i = 3; $i < 1003; $i++) {
			$md[$i] = hash($algo, $md[$i-3].$md[$i-2].$md[$i-1], true);
		}
		$seed = $md[1002];
		echo "{$algo}: ", bin2hex($seed), " (checkpoint {$j})\n";
	}
}

// FIPS 180-2 contains all three
// FIPS 180-3 links to a larger directory that only has the first two
//	https://csrc.nist.gov/projects/cryptographic-standards-and-guidelines/example-values
// RFC 3874 also covers sha224 with the same 3 vectors
echo "*** FIPS 180-2 and later ***\n";
foreach (["sha224", "sha256", "sha384", "sha512", "sha512/224", "sha512/256"] as $algo) {
	test($algo, "abc");
	test($algo, ($algo[4] === '2')
		? "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
		: "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"
	);
	// Only part of FIPS 180-2, which didn't include the sha512/t variants
	isset($algo[6]) OR test($algo, str_repeat("a", 1000000));
}

// Linked from the example-values above, excluding the extremely large vectors
//	https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/SHA2_Additional.pdf
echo "*** NIST SHA2 Additional ***\n";
test("sha224", "\xff");
test("sha224", "\xe5\xe0\x99\x24");
test("sha224", str_repeat("\x00", 56));
test("sha224", str_repeat("Q", 1000));
test("sha224", str_repeat("A", 1000));
test("sha224", str_repeat("\x99", 1005));
test("sha224", str_repeat("\x00", 1000000));
test("sha256", "\xbd");
foreach ([55, 56, 57, 64, 1000] as $zeros) {
	test("sha256", str_repeat("\x00", $zeros));
}
test("sha256", str_repeat("A", 1000));
test("sha256", str_repeat("U", 1005));
test("sha256", str_repeat("\x00", 1000000));
foreach (["sha384", "sha512"] as $algo) {
	test($algo, "");
	foreach ([111, 112, 113, 122, 1000] as $zeros) {
		test($algo, str_repeat("\x00", $zeros));
	}
	test($algo, str_repeat("A", 1000));
	test($algo, str_repeat("U", 1005));
	test($algo, str_repeat("\x00", 1000000));
}

// IETF RFCs also include the 3 FIPS 180-2 samples from above
echo "*** RFC 4634 / RFC 6234 ***\n";
test("sha224", str_repeat("01234567012345670123456701234567", 20));
test("sha224", "\x07");
test("sha224", "\x18\x80\x40\x05\xdd\x4f\xbd\x15\x56\x29\x9d\x6f\x9d\x93\xdf\x62");
test("sha224",
	"\x55\xb2\x10\x07\x9c\x61\xb5\x3a\xdd\x52\x06\x22\xd1\xac\x97\xd5\xcd\xbe\x8c\xb3\x3a\xa0\xae\x34\x45\x17\xbe\xe4\xd7\xba\x09\xab" .
	"\xc8\x53\x3c\x52\x50\x88\x7a\x43\xbe\xbb\xac\x90\x6c\x2e\x18\x37\xf2\x6b\x36\xa5\x9a\xe3\xbe\x78\x14\xd5\x06\x89\x6b\x71\x8b\x2a" .
	"\x38\x3e\xcd\xac\x16\xb9\x61\x25\x55\x3f\x41\x6f\xf3\x2c\x66\x74\xc7\x45\x99\xa9\x00\x53\x86\xd9\xce\x11\x12\x24\x5f\x48\xee\x47" .
	"\x0d\x39\x6c\x1e\xd6\x3b\x92\x67\x0c\xa5\x6e\xc8\x4d\xee\xa8\x14\xb6\x13\x5e\xca\x54\x39\x2b\xde\xdb\x94\x89\xbc\x9b\x87\x5a\x8b" .
	"\xaf\x0d\xc1\xae\x78\x57\x36\x91\x4a\xb7\xda\xa2\x64\xbc\x07\x9d\x26\x9f\x2c\x0d\x7e\xdd\xd8\x10\xa4\x26\x14\x5a\x07\x76\xf6\x7c" .
	"\x87\x82\x73"
);
monteCarloTest("sha224", hex2bin("d0569cb3665a8a43eb6ea23d75a3c4d2054a0d7d66a9ca99c9ceb027"));
test("sha256", str_repeat("01234567012345670123456701234567", 20));
test("sha256", "\x19");
test("sha256", "\xe3\xd7\x25\x70\xdc\xdd\x78\x7c\xe3\x88\x7a\xb2\xcd\x68\x46\x52");
test("sha256",
	"\x83\x26\x75\x4e\x22\x77\x37\x2f\x4f\xc1\x2b\x20\x52\x7a\xfe\xf0\x4d\x8a\x05\x69\x71\xb1\x1a\xd5\x71\x23\xa7\xc1\x37\x76\x00\x00" .
	"\xd7\xbe\xf6\xf3\xc1\xf7\xa9\x08\x3a\xa3\x9d\x81\x0d\xb3\x10\x77\x7d\xab\x8b\x1e\x7f\x02\xb8\x4a\x26\xc7\x73\x32\x5f\x8b\x23\x74" .
	"\xde\x7a\x4b\x5a\x58\xcb\x5c\x5c\xf3\x5b\xce\xe6\xfb\x94\x6e\x5b\xd6\x94\xfa\x59\x3a\x8b\xeb\x3f\x9d\x65\x92\xec\xed\xaa\x66\xca" .
	"\x82\xa2\x9d\x0c\x51\xbc\xf9\x33\x62\x30\xe5\xd7\x84\xe4\xc0\xa4\x3f\x8d\x79\xa3\x0a\x16\x5c\xba\xbe\x45\x2b\x77\x4b\x9c\x71\x09" .
	"\xa9\x7d\x13\x8f\x12\x92\x28\x96\x6f\x6c\x0a\xdc\x10\x6a\xad\x5a\x9f\xdd\x30\x82\x57\x69\xb2\xc6\x71\xaf\x67\x59\xdf\x28\xeb\x39" .
	"\x3d\x54\xd6"
);
monteCarloTest("sha256", hex2bin("f41ece2613e4573915696b5adcd51ca328be3bf566a9ca99c9ceb0279c1cb0a7"));
test("sha384", str_repeat("01234567012345670123456701234567", 20));
test("sha384", "\xb9");
test("sha384", "\xa4\x1c\x49\x77\x79\xc0\x37\x5f\xf1\x0a\x7f\x4e\x08\x59\x17\x39");
test("sha384",
	"\x39\x96\x69\xe2\x8f\x6b\x9c\x6d\xbc\xbb\x69\x12\xec\x10\xff\xcf\x74\x79\x03\x49\xb7\xdc\x8f\xbe\x4a\x8e\x7b\x3b\x56\x21\xdb\x0f" .
	"\x3e\x7d\xc8\x7f\x82\x32\x64\xbb\xe4\x0d\x18\x11\xc9\xea\x20\x61\xe1\xc8\x4a\xd1\x0a\x23\xfa\xc1\x72\x7e\x72\x02\xfc\x3f\x50\x42" .
	"\xe6\xbf\x58\xcb\xa8\xa2\x74\x6e\x1f\x64\xf9\xb9\xea\x35\x2c\x71\x15\x07\x05\x3c\xf4\xe5\x33\x9d\x52\x86\x5f\x25\xcc\x22\xb5\xe8" .
	"\x77\x84\xa1\x2f\xc9\x61\xd6\x6c\xb6\xe8\x95\x73\x19\x9a\x2c\xe6\x56\x5c\xbd\xf1\x3d\xca\x40\x38\x32\xcf\xcb\x0e\x8b\x72\x11\xe8" .
	"\x3a\xf3\x2a\x11\xac\x17\x92\x9f\xf1\xc0\x73\xa5\x1c\xc0\x27\xaa\xed\xef\xf8\x5a\xad\x7c\x2b\x7c\x5a\x80\x3e\x24\x04\xd9\x6d\x2a" .
	"\x77\x35\x7b\xda\x1a\x6d\xae\xed\x17\x15\x1c\xb9\xbc\x51\x25\xa4\x22\xe9\x41\xde\x0c\xa0\xfc\x50\x11\xc2\x3e\xcf\xfe\xfd\xd0\x96" .
	"\x76\x71\x1c\xf3\xdb\x0a\x34\x40\x72\x0e\x16\x15\xc1\xf2\x2f\xbc\x3c\x72\x1d\xe5\x21\xe1\xb9\x9b\xa1\xbd\x55\x77\x40\x86\x42\x14" .
	"\x7e\xd0\x96"
);
monteCarloTest("sha384", hex2bin("8240bc51e4ec7ef76d18e35204a19f51a5213a73a81d6f944680d3075948b7e463804ea3d26e13ea820d65a484be7453"));
test("sha512", str_repeat("01234567012345670123456701234567", 20));
test("sha512", "\xd0");
test("sha512", "\x8d\x4e\x3c\x0e\x38\x89\x19\x14\x91\x81\x6e\x9d\x98\xbf\xf0\xa0");
test("sha512",
	"\xa5\x5f\x20\xc4\x11\xaa\xd1\x32\x80\x7a\x50\x2d\x65\x82\x4e\x31\xa2\x30\x54\x32\xaa\x3d\x06\xd3\xe2\x82\xa8\xd8\x4e\x0d\xe1\xde" .
	"\x69\x74\xbf\x49\x54\x69\xfc\x7f\x33\x8f\x80\x54\xd5\x8c\x26\xc4\x93\x60\xc3\xe8\x7a\xf5\x65\x23\xac\xf6\xd8\x9d\x03\xe5\x6f\xf2" .
	"\xf8\x68\x00\x2b\xc3\xe4\x31\xed\xc4\x4d\xf2\xf0\x22\x3d\x4b\xb3\xb2\x43\x58\x6e\x1a\x7d\x92\x49\x36\x69\x4f\xcb\xba\xf8\x8d\x95" .
	"\x19\xe4\xeb\x50\xa6\x44\xf8\xe4\xf9\x5e\xb0\xea\x95\xbc\x44\x65\xc8\x82\x1a\xac\xd2\xfe\x15\xab\x49\x81\x16\x4b\xbb\x6d\xc3\x2f" .
	"\x96\x90\x87\xa1\x45\xb0\xd9\xcc\x9c\x67\xc2\x2b\x76\x32\x99\x41\x9c\xc4\x12\x8b\xe9\xa0\x77\xb3\xac\xe6\x34\x06\x4e\x6d\x99\x28" .
	"\x35\x13\xdc\x06\xe7\x51\x5d\x0d\x73\x13\x2e\x9a\x0d\xc6\xd3\xb1\xf8\xb2\x46\xf1\xa9\x8a\x3f\xc7\x29\x41\xb1\xe3\xbb\x20\x98\xe8" .
	"\xbf\x16\xf2\x68\xd6\x4f\x0b\x0f\x47\x07\xfe\x1e\xa1\xa1\x79\x1b\xa2\xf3\xc0\xc7\x58\xe5\xf5\x51\x86\x3a\x96\xc9\x49\xad\x47\xd7" .
	"\xfb\x40\xd2"
);
monteCarloTest("sha512", hex2bin("473ff1b9b3ffdfa126699ac7ef9e8e787773095824c642557c1399d98e4220448dc35b99bfdd44779543924c1ce93bc5941538895db988261b00774b12272039"));

?>
--EXPECT--
*** FIPS 180-2 and later ***
sha224: 23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7
sha224: 75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525
sha224: 20794655980c91d8bbb4c1ea97618a4bf03f42581948b2ee4ee7ad67
sha256: ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
sha256: cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1
sha256: cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0
sha384: cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7
sha384: 09330c33f71147e83d192fc782cd1b4753111b173b3b05d22fa08086e3b0f712fcc7c71a557e2db966c3e9fa91746039
sha384: 9d0e1809716474cb086e834e310a4a1ced149e9c00f248527972cec5704c2a5b07b8b3dc38ecc4ebae97ddd87f3d8985
sha512: ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f
sha512: 8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909
sha512: e718483d0ce769644e2e42c7bc15b4638e1f98b13b2044285632a803afa973ebde0ff244877ea60a4cb0432ce577c31beb009c5c2c49aa2e4eadb217ad8cc09b
sha512/224: 4634270f707b6a54daae7530460842e20e37ed265ceee9a43e8924aa
sha512/224: 23fec5bb94d60b23308192640b0c453335d664734fe40e7268674af9
sha512/256: 53048e2681941ef99b2e29b76b4c7dabe4c2d0c634fc6d46e0e2f13107e7af23
sha512/256: 3928e184fb8690f840da3988121d31be65cb9d3ef83ee6146feac861e19b563a
*** NIST SHA2 Additional ***
sha224: e33f9d75e6ae1369dbabf81b96b4591ae46bba30b591a6b6c62542b5
sha224: fd19e74690d291467ce59f077df311638f1c3a46e510d0e49a67062d
sha224: 5c3e25b69d0ea26f260cfae87e23759e1eca9d1ecc9fbf3c62266804
sha224: 3706197f66890a41779dc8791670522e136fafa24874685715bd0a8a
sha224: a8d0c66b5c6fdfd836eb3c6d04d32dfe66c3b1f168b488bf4c9c66ce
sha224: cb00ecd03788bf6c0908401e0eb053ac61f35e7e20a2cfd7bd96d640
sha224: 3a5d74b68f14f3a4b2be9289b8d370672d0b3d2f53bc303c59032df3
sha256: 68325720aabd7c82f30f554b313d0570c95accbb7dc4b5aae11204c08ffe732b
sha256: 02779466cdec163811d078815c633f21901413081449002f24aa3e80f0b88ef7
sha256: d4817aa5497628e7c77e6b606107042bbba3130888c5f47a375e6179be789fbb
sha256: 65a16cb7861335d5ace3c60718b5052e44660726da4cd13bb745381b235a1785
sha256: f5a5fd42d16a20302798ef6ed309979b43003d2320d9f0e8ea9831a92759fb4b
sha256: 541b3e9daa09b20bf85fa273e5cbd3e80185aa4ec298e765db87742b70138a53
sha256: c2e686823489ced2017f6059b8b239318b6364f6dcd835d0a519105a1eadd6e4
sha256: f4d62ddec0f3dd90ea1380fa16a5ff8dc4c54b21740650f24afc4120903552b0
sha256: d29751f2649b32ff572b5e0a9f541ea660a50f94ff0beedfb0b692b924cc8025
sha384: 38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b
sha384: 435770712c611be7293a66dd0dc8d1450dc7ff7337bfe115bf058ef2eb9bed09cee85c26963a5bcc0905dc2df7cc6a76
sha384: 3e0cbf3aee0e3aa70415beae1bd12dd7db821efa446440f12132edffce76f635e53526a111491e75ee8e27b9700eec20
sha384: 6be9af2cf3cd5dd12c8d9399ec2b34e66034fbd699d4e0221d39074172a380656089caafe8f39963f94cc7c0a07e3d21
sha384: 12a72ae4972776b0db7d73d160a15ef0d19645ec96c7f816411ab780c794aa496a22909d941fe671ed3f3caee900bdd5
sha384: aae017d4ae5b6346dd60a19d52130fb55194b6327dd40b89c11efc8222292de81e1a23c9b59f9f58b7f6ad463fa108ca
sha384: 7df01148677b7f18617eee3a23104f0eed6bb8c90a6046f715c9445ff43c30d69e9e7082de39c3452fd1d3afd9ba0689
sha384: 1bb8e256da4a0d1e87453528254f223b4cb7e49c4420dbfa766bba4adba44eeca392ff6a9f565bc347158cc970ce44ec
sha384: 8a1979f9049b3fff15ea3a43a4cf84c634fd14acad1c333fecb72c588b68868b66a994386dc0cd1687b9ee2e34983b81
sha512: cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e
sha512: 77ddd3a542e530fd047b8977c657ba6ce72f1492e360b2b2212cd264e75ec03882e4ff0525517ab4207d14c70c2259ba88d4d335ee0e7e20543d22102ab1788c
sha512: 2be2e788c8a8adeaa9c89a7f78904cacea6e39297d75e0573a73c756234534d6627ab4156b48a6657b29ab8beb73334040ad39ead81446bb09c70704ec707952
sha512: 0e67910bcf0f9ccde5464c63b9c850a12a759227d16b040d98986d54253f9f34322318e56b8feb86c5fb2270ed87f31252f7f68493ee759743909bd75e4bb544
sha512: 4f3f095d015be4a7a7cc0b8c04da4aa09e74351e3a97651f744c23716ebd9b3e822e5077a01baa5cc0ed45b9249e88ab343d4333539df21ed229da6f4a514e0f
sha512: ca3dff61bb23477aa6087b27508264a6f9126ee3a004f53cb8db942ed345f2f2d229b4b59c859220a1cf1913f34248e3803bab650e849a3d9a709edc09ae4a76
sha512: 329c52ac62d1fe731151f2b895a00475445ef74f50b979c6f7bb7cae349328c1d4cb4f7261a0ab43f936a24b000651d4a824fcdd577f211aef8f806b16afe8af
sha512: 59f5e54fe299c6a8764c6b199e44924a37f59e2b56c3ebad939b7289210dc8e4c21b9720165b0f4d4374c90f1bf4fb4a5ace17a1161798015052893a48c3d161
sha512: ce044bc9fd43269d5bbc946cbebc3bb711341115cc4abdf2edbc3ff2c57ad4b15deb699bda257fea5aef9c6e55fcf4cf9dc25a8c3ce25f2efe90908379bff7ed
*** RFC 4634 / RFC 6234 ***
sha224: 567f69f168cd7844e65259ce658fe7aadfa25216e68eca0eb7ab8262
sha224: 00ecd5f138422b8ad74c9799fd826c531bad2fcabc7450bee2aa8c2a
sha224: df90d78aa78821c99b40ba4c966921accd8ffb1e98ac388e56191db1
sha224: 0b31894ec8937ad9b91bdfbcba294d9adefaa18e09305e9f20d5c3a4
sha224: 100966a5b4fde0b42e2a6c5953d4d7f41ba7cf79fd2df431416734be (checkpoint 0)
sha224: 1dca396b0c417715defaae9641e10a2e99d55abcb8a00061eb3be8bd (checkpoint 1)
sha224: 1864e627bdb2319973cd5ed7d68da71d8bf0f983d8d9ab32c34adb34 (checkpoint 2)
sha224: a2406481fc1bcaf24dd08e6752e844709563fb916227fed598eb621f (checkpoint 3)
sha256: 594847328451bdfa85056225462cc1d867d877fb388df0ce35f25ab5562bfbb5
sha256: 68aa2e2ee5dff96e3355e6c7ee373e3d6a4e17f75f9518d843709c0c9bc3e3d4
sha256: 175ee69b02ba9b58e2b0a5fd13819cea573f3940a94f825128cf4209beabb4e8
sha256: 97dbca7df46d62c8a422c941dd7e835b8ad3361763f7e9b2d95f4f0da6e1ccbc
sha256: 83d28614d49c3adc1d6fc05db5f48037c056f8d2a4ce44ec6457dea5dd797cd1 (checkpoint 0)
sha256: 99dbe3127ef2e93dd9322d6a07909eb33b63995e529b3f954b8581621bb74d39 (checkpoint 1)
sha256: 8d4be295bb64661ca3c7efd129a2f725b33072dbdde32385b9a87b9af88ea76f (checkpoint 2)
sha256: 40af5d3f9716b040df9408e31536b70ff906ec51b00447ca97d7dd97c12411f4 (checkpoint 3)
sha384: 2fc64a4f500ddb6828f6a3430b8dd72a368eb7f3a8322a70bc84275b9c0b3ab00d27a5cc3c2d224aa6b61a0d79fb4596
sha384: bc8089a19007c0b14195f4ecc74094fec64f01f90929282c2fb392881578208ad466828b1c6c283d2722cf0ad1ab6938
sha384: c9a68443a005812256b8ec76b00516f0dbb74fab26d665913f194b6ffb0e91ea9967566b58109cbc675cc208e4c823f7
sha384: 4f440db1e6edd2899fa335f09515aa025ee177a79f4b4aaf38e42b5c4de660f5de8fb2a5b2fbd2a3cbffd20cff1288c0
sha384: ce44d7d63ae0c91482998cf662a51ec80bf6fc68661a3c57f87566112bd635a743ea904deb7d7a42ac808cabe697f38f (checkpoint 0)
sha384: f9c6d261881fee41acd39e67aa8d0bad507c7363eb67e2b81f45759f9c0fd7b503df1a0b9e80bde7bc333d75b804197d (checkpoint 1)
sha384: d96512d8c9f4a7a4967a366c01c6fd97384225b58343a88264847c18e4ef8ab7aee4765ffbc3e30bd485d3638a01418f (checkpoint 2)
sha384: 0ca76bd0813af1509e170907a96005938bc985628290b25fef73cf6fad68ddba0ac8920c94e0541607b0915a7b4457f7 (checkpoint 3)
sha512: 89d05ba632c699c31231ded4ffc127d5a894dad412c0e024db872d1abd2ba8141a0f85072a9be1e2aa04cf33c765cb510813a39cd5a84c4acaa64d3f3fb7bae9
sha512: 9992202938e882e73e20f6b69e68a0a7149090423d93c81bab3f21678d4aceeee50e4e8cafada4c85a54ea8306826c4ad6e74cece9631bfa8a549b4ab3fbba15
sha512: cb0b67a4b8712cd73c9aabc0b199e9269b20844afb75acbdd1c153c9828924c3ddedaafe669c5fdd0bc66f630f6773988213eb1b16f517ad0de4b2f0c95c90f8
sha512: c665befb36da189d78822d10528cbf3b12b3eef726039909c1a16a270d48719377966b957a878e720584779a62825c18da26415e49a7176a894e7510fd1451f5
sha512: 2fbb1e7e00f746ba514fbc8c421f36792ec0e11ff5efc378e1ab0c079aa5f0f66a1e3edbaeb4f9984be14437123038a452004a55768c1fd8eed49e4a21bedcd0 (checkpoint 0)
sha512: 25cbe5a4f2c7b1d7ef07011705d50c62c5000594243eafd1241fc9f3d22b58184ae2fee38e171cf8129e29459c9bc2ef461af5708887315f15419d8d17fe7949 (checkpoint 1)
sha512: 5b8b1f2687555ce2d7182b92e5c3f6c36547da1c13dbb9ea4f73ea4cbbaf89411527906d35b1b06c1b6a8007d05ec66df0a406066829eab618bde3976515aafc (checkpoint 2)
sha512: 46e36b007d19876cdb0b29ad074fe3c08cdd174d42169d6abe5a1414b6e79707df58776a98091cf431854147bb6d3c66d43bfbc108fd715bde6aa127c2b0e79f (checkpoint 3)
