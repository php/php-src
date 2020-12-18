--TEST--
Hash: MurmurHash3 seed
--FILE--
<?php

$ctx = hash_init("murmur3f", options: ["seed" => 42]);
hash_update($ctx, "Two");
hash_update($ctx, " hashes");
hash_update($ctx, " meet");
hash_update($ctx, " in");
hash_update($ctx, " a");
hash_update($ctx, " bar.");
$h0 = hash_final($ctx);
echo $h0, "\n";

$h0 = hash("murmur3f", "Two hashes meet in a bar.", options: ["seed" => 42]);
echo $h0, "\n";

$ctx = hash_init("murmur3c", options: ["seed" => 106]);
hash_update($ctx, "Two");
hash_update($ctx, " hashes");
hash_update($ctx, " meet");
hash_update($ctx, " in");
hash_update($ctx, " a");
hash_update($ctx, " bar.");
$h0 = hash_final($ctx);
echo $h0, "\n";

$h0 = hash("murmur3c", "Two hashes meet in a bar.", options: ["seed" => 106]);
echo $h0, "\n";

$ctx = hash_init("murmur3a", options: ["seed" => 2345]);
hash_update($ctx, "Two");
hash_update($ctx, " hashes");
hash_update($ctx, " meet");
hash_update($ctx, " in");
hash_update($ctx, " a");
hash_update($ctx, " bar.");
$h0 = hash_final($ctx);
echo $h0, "\n";

$h0 = hash("murmur3a", "Two hashes meet in a bar.", options: ["seed" => 2345]);
echo $h0, "\n";

?>
--EXPECT--
95855f9be0db784a5c37e878c4a4dcee
95855f9be0db784a5c37e878c4a4dcee
f64c9eb40287fa686575163893e283b2
f64c9eb40287fa686575163893e283b2
7f7ec59b
7f7ec59b
