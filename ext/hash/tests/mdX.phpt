--TEST--
Hash: MD2, MD4, MD5 algorithms
--FILE--
<?php
$vectors = [
	"",
	"a",
	"abc",
	"message digest",
	"abcdefghijklmnopqrstuvwxyz",
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
	str_repeat("1234567890", 8)
];

echo "*** RFC 1319 / RFC 1320 / RFC 1321 ***\n";
foreach (["md2", "md4", "md5"] as $algo) {
	foreach ($vectors as $vector) {
		echo "{$algo}: ", hash($algo, $vector), "\n";
	}
}
?>
--EXPECT--
*** RFC 1319 / RFC 1320 / RFC 1321 ***
md2: 8350e5a3e24c153df2275c9f80692773
md2: 32ec01ec4a6dac72c0ab96fb34c0b5d1
md2: da853b0d3f88d99b30283a69e6ded6bb
md2: ab4f496bfb2a530b219ff33031fe06b0
md2: 4e8ddff3650292ab5a4108c3aa47940b
md2: da33def2a42df13975352846c30338cd
md2: d5976f79d83d3a0dc9806c3c66f3efd8
md4: 31d6cfe0d16ae931b73c59d7e0c089c0
md4: bde52cb31de33e46245e05fbdbd6fb24
md4: a448017aaf21d8525fc10ae87aa6729d
md4: d9130a8164549fe818874806e1c7014b
md4: d79e1c308aa5bbcdeea8ed63df412da9
md4: 043f8582f241db351ce627e153e7f0e4
md4: e33b4ddc9c38f2199c3e7b164fcc0536
md5: d41d8cd98f00b204e9800998ecf8427e
md5: 0cc175b9c0f1b6a831c399e269772661
md5: 900150983cd24fb0d6963f7d28e17f72
md5: f96b697d7cb7938d525a2f31aaf161d0
md5: c3fcd3d76192e4007dfb496cca67e13b
md5: d174ab98d277d9f5a5611c2c9f419d9f
md5: 57edf4a22be3c955ac49da2e2107b67a
