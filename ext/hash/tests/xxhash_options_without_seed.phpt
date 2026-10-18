--TEST--
Hash: xxHash options without a seed do not emit a deprecation
--FILE--
<?php

foreach (["xxh32", "xxh64", "xxh3", "xxh128"] as $a) {
	var_dump(hash($a, "x", options: []) === hash($a, "x"));
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
