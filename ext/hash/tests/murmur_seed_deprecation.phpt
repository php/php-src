--TEST--
Hash: murmur3 seed deprecation of edge cases
--FILE--
<?php

foreach (["murmur3a", "murmur3c", "murmur3f"] as $a) {
	hash_init($a, options: ["seed" => "42"]);
}

?>
--EXPECTF--
Deprecated: hash_init(): Passing a seed of a type other than int is deprecated because it is the same as setting the seed to 0 in %s on line %d

Deprecated: hash_init(): Passing a seed of a type other than int is deprecated because it is the same as setting the seed to 0 in %s on line %d

Deprecated: hash_init(): Passing a seed of a type other than int is deprecated because it is the same as setting the seed to 0 in %s on line %d
