--TEST--
Hash: MurmurHash3 test
--FILE--
<?php

$h = hash("murmur3a", "foo");
echo $h, "\n";

$h = hash("murmur3c", "Two hashes meet in a bar", false);
echo $h, "\n";
$h = hash("murmur3c", "hash me!");
echo $h, "\n";

$h = hash("murmur3f", "Two hashes meet in a bar", false);
echo $h, "\n";
$h = hash("murmur3f", "hash me!");
echo $h, "\n";

$ctx = hash_init("murmur3a");
hash_update($ctx, "hello");
hash_update($ctx, " there");
$h0 = hash_final($ctx);
$h1 = hash("murmur3a", "hello there");
echo $h0, " ", $h1, "\n";

$ctx = hash_init("murmur3c");
hash_update($ctx, "hello");
hash_update($ctx, " there");
$h0 = hash_final($ctx);
$h1 = hash("murmur3c", "hello there");
echo $h0, " ", $h1, "\n";

$ctx = hash_init("murmur3f");
hash_update($ctx, "hello");
hash_update($ctx, " there");
$h0 = hash_final($ctx);
$h1 = hash("murmur3f", "hello there");
echo $h0, " ", $h1, "\n";

?>
--EXPECT--
f6a5c420
8036c2707453c6f37348142be7eaf75c
c7009299985a5627a9280372a9280372
40256ed26fa6ece7785092ed33c8b659
c43668294e89db0ba5772846e5804467
6440964d 6440964d
2bcadca212d62deb69712a721e593089 2bcadca212d62deb69712a721e593089
81514cc240f57a165c95eb63f9c0eedf 81514cc240f57a165c95eb63f9c0eedf
