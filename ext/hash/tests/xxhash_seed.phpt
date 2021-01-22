--TEST--
Hash: xxHash seed
--FILE--
<?php

foreach (["xxh32", "xxh64", "xxh3", "xxh128"] as $a) {

	$ctx = hash_init($a, options: ["seed" => 42]);
	hash_update($ctx, "Lorem");
	hash_update($ctx, " ipsum dolor");
	hash_update($ctx, " sit amet,");
	hash_update($ctx, " consectetur adipiscing elit.");
	$h0 = hash_final($ctx);
	echo $h0, "\n";

	$h0 = hash($a, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.", options: ["seed" => 42]);
	echo $h0, "\n";
}

?>
--EXPECT--
3d0cc7e5
3d0cc7e5
9c9aa071b5d22a15
9c9aa071b5d22a15
366409913c16b70d
366409913c16b70d
f87856a7589354e92aeca886c71ed7fb
f87856a7589354e92aeca886c71ed7fb
