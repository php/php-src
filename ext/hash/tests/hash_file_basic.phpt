--TEST--
Hash: hash_file() basic functionality
--FILE--
<?php
echo "*** Testing hash_file() basic functionality ***\n";

$hash = hash_file("md5", __FILE__);
echo "Default output is hex-encoded: ", var_export((bool) strspn($hash, "0123456789abcdef"), true), "\n";
$hex = hash_file("md5", __FILE__, false);
$bin = hash_file("md5", __FILE__, true);
echo "Hex output equals bin2hex(binary): ", var_export($hex === bin2hex($bin), true), "\n";

$data = include(__DIR__."/samples.inc");
list($message, $digest) = [$data["message"], $data["digest"]];
$file = __DIR__."hash_file.txt";
if (false === file_put_contents($file, $message)) {
	echo "ERROR: Cannot create file {$file}\n";
	exit;
}

echo "*** Outputs for content '{$message}' ***\n";
foreach (hash_algos() as $algo) {
	echo "{$algo}: ", $hash = hash_file($algo, $file), "\n";
	if ($hash !== $digest[$algo]) {
		echo "{$algo} EXPECTED {$digest[$algo]}\n";
	}
}
unlink($file);
?>
--EXPECT--
*** Testing hash_file() basic functionality ***
Default output is hex-encoded: true
Hex output equals bin2hex(binary): true
*** Outputs for content 'The quick brown fox jumps over the lazy dog' ***
md2: 03d85a0d629d2c442e987525319fc471
md4: 1bee69a46ba811185c194762abaeae90
md5: 9e107d9d372bb6826bd81d3542a419d6
sha1: 2fd4e1c67a2d28fced849ee1bb76e7391b93eb12
sha224: 730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525
sha256: d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592
sha384: ca737f1014a48f4c0b6dd43cb177b0afd9e5169367544c494011e3317dbf9a509cb1e5dc1e85a941bbee3d7f2afbc9b1
sha512/224: 944cd2847fb54558d4775db0485a50003111c8e5daa63fe722c6aa37
sha512/256: dd9d67b371519c339ed8dbd25af90e976a1eeefd4ad3d889005e532fc5bef04d
sha512: 07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6
sha3-224: d15dadceaa4d5d7bb3b48f446421d542e08ad8887305e28d58335795
sha3-256: 69070dda01975c8c120c3aada1b282394e7f032fa9cf32f4cb2259a0897dfc04
sha3-384: 7063465e08a93bce31cd89d2e3ca8f602498696e253592ed26f07bf7e703cf328581e1471a7ba7ab119b1a9ebdf8be41
sha3-512: 01dedd5de4ef14642445ba5f5b97c15e47b9ad931326e4b0727cd94cefc44fff23f07bf543139939b49128caf436dc1bdee54fcb24023a08d9403f9b4bf0d450
ripemd128: 3fa9b57f053c053fbe2735b2380db596
ripemd160: 37f332f68db77bd9d7edd4969571ad671cf9dd3b
ripemd256: c3b0c2f764ac6d576a6c430fb61a6f2255b4fa833e094b1ba8c1e29b6353036f
ripemd320: e7660e67549435c62141e51c9ab1dcc3b1ee9f65c0b3e561ae8f58c5dba3d21997781cd1cc6fbc34
whirlpool: b97de512e91e3828b40d2b0fdce9ceb3c4a71f9bea8d88e75c4fa854df36725fd2b52eb6544edcacd6f8beddfea403cb55ae31f03ad62a5ef54e42ee82c3fb35
tiger128,3: 6d12a41e72e644f017b6f0e2f7b44c62
tiger160,3: 6d12a41e72e644f017b6f0e2f7b44c6285f06dd5
tiger192,3: 6d12a41e72e644f017b6f0e2f7b44c6285f06dd5d2c5b075
tiger128,4: c1f3a704e9f6267e9f75fa47191f83c3
tiger160,4: c1f3a704e9f6267e9f75fa47191f83c354100a04
tiger192,4: c1f3a704e9f6267e9f75fa47191f83c354100a04c4f1dc6f
snefru: 674caa75f9d8fd2089856b95e93a4fb42fa6c8702f8980e11d97a142d76cb358
snefru256: 674caa75f9d8fd2089856b95e93a4fb42fa6c8702f8980e11d97a142d76cb358
gost: 77b7fa410c9ac58a25f49bca7d0468c9296529315eaca76bd1a10f376d1f4294
gost-crypto: 9004294a361a508c586fe53d1f1b02746765e71b765472786e4770d565830a76
adler32: 5bdc0fda
crc32: 61ee9d45
crc32b: 414fa339
crc32c: 22620404
fnv132: e9c86c6e
fnv1a32: 048fff90
fnv164: a8b2f3117de37ace
fnv1a64: f3f9b7f5e7e47110
joaat: 519e91f5
murmur3a: 2e4ff723
murmur3c: 2f1583c3ecee2c675d7bf66ce5e91d2c
murmur3f: e34bbc7bbc071b6c7a433ca9c49a9347
xxh32: e85ea4de
xxh64: 0b242d361fda71bc
xxh3: ce7d19a5418fb365
xxh128: ddd650205ca3e7fa24a1cc2e3a8a7651
haval128,3: 713502673d67e5fa557629a71d331945
haval160,3: b338ac397e8bccadcccd96549cadd4882d834107
haval192,3: 58e6ced002e311172483d434ba738ad033e7fa950e431503
haval224,3: e1d5792306f56b22419662b06d1885a66dca3eba01f53274c89aeaeb
haval256,3: 9446028f42b3768a41bd873ca69b0c006341d986613567f39eb61f96ca683300
haval128,4: 6eece560a2e8d6b919e81fe91b0e7156
haval160,4: 6e739d01f5739ceed94da1a115b52d5951280560
haval192,4: 228ee09bc7e36151c6f285f558e6aede66ad38c8341592b9
haval224,4: dddd6689885f6db4ad91e35a35e1f4498446510df798d4fd54b8654f
haval256,4: c0d4c6ea514105fd1a9c38a238553fb7fa21d4127eb1a3035a75ce9d06a83d96
haval128,5: 696f02111f2e1da5c21d50eb782b7e8f
haval160,5: ecce9fa8a428866304ff082af2f9062637d36b23
haval192,5: 023d045f75d4bf051fd6e50f7b7417bf9949c4b5d2b4b7ef
haval224,5: 03d953298c8e56b46385c6761cd4b2e377889a75c97eaea475421c73
haval256,5: b89c551cdfe2e06dbd4cea2be1bc7d557416c58ebb4d07cbc94e49f710c55be4
