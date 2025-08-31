--TEST--
Hash: xxHash seed deprecation of edge cases
--FILE--
<?php

foreach (["xxh32", "xxh64", "xxh3", "xxh128"] as $a) {
	hash_init($a, options: ["seed" => "42"]);
}

foreach (["xxh3", "xxh128"] as $a) {
    try {
	    hash_init($a, options: ["secret" => 42]);
	} catch (Throwable) {}
}

?>
--EXPECTF--
Deprecated: hash_init(): Passing a seed of a type other than int is deprecated because it is the same as setting the seed to 0 in %s on line %d

Deprecated: hash_init(): Passing a seed of a type other than int is deprecated because it is the same as setting the seed to 0 in %s on line %d

Deprecated: hash_init(): Passing a seed of a type other than int is deprecated because it is ignored in %s on line %d

Deprecated: hash_init(): Passing a seed of a type other than int is deprecated because it is ignored in %s on line %d

Deprecated: hash_init(): Passing a secret of a type other than string is deprecated because it implicitly converts to a string, potentially hiding bugs in %s on line %d

Deprecated: hash_init(): Passing a secret of a type other than string is deprecated because it implicitly converts to a string, potentially hiding bugs in %s on line %d
