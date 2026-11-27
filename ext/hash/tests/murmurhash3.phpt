--TEST--
Hash: MurmurHash3 algorithm
--FILE--
<?php
$algos = ["murmur3a", "murmur3c", "murmur3f"];

foreach ($algos as $algo) {
	echo "{$algo}: ", hash($algo, "Two hashes meet in a bar", false), "\n";
	echo "{$algo}: ", hash($algo, "hash me!"), "\n";

	$ctx = hash_init($algo);
	hash_update($ctx, "hello");
	hash_update($ctx, " there");
	echo "{$algo}: ", hash_final($ctx), " ", hash($algo, "hello there"), "\n";
}

// https://github.com/aappleby/smhasher/blob/92cf3702fcfaadc84eb7bef59825a23e0cd84f56/src/main.cpp#L71
// https://github.com/aappleby/smhasher/blob/92cf3702fcfaadc84eb7bef59825a23e0cd84f56/src/KeysetTest.cpp#L13
echo "*** SMHasher reference ***\n";
$key = pack("C*", ... range(0, 255));
foreach ($algos as $algo) {
	$hash = '';
	for ($i = 0; $i < 256; $i++) {
		$words = unpack("N*", hash($algo, substr($key, 0, $i), true, ["seed" => 256 - $i]));
		// Needs to be in in little-endian order
		$hash .= ($algo === "murmur3f")
			? pack("V*", $words[2], $words[1], $words[4], $words[3])
			: pack("V*", ... $words);
	}

	$hash = hash($algo, $hash, false, ["seed" => 0]);
	$hash = substr($hash, ($algo === "murmur3f" ? 8 : 0), 8);
	echo "{$algo}: {$hash}\n";
}
?>
--EXPECT--
murmur3a: fe854d39
murmur3a: 7adaaf4e
murmur3a: 6440964d 6440964d
murmur3c: 8036c2707453c6f37348142be7eaf75c
murmur3c: c7009299985a5627a9280372a9280372
murmur3c: 2bcadca212d62deb69712a721e593089 2bcadca212d62deb69712a721e593089
murmur3f: 40256ed26fa6ece7785092ed33c8b659
murmur3f: c43668294e89db0ba5772846e5804467
murmur3f: 81514cc240f57a165c95eb63f9c0eedf 81514cc240f57a165c95eb63f9c0eedf
*** SMHasher reference ***
murmur3a: b0f57ee3
murmur3c: b3ece62a
murmur3f: 6384ba69
