--TEST--
GH-12073: Freeing of non-ZMM pointer of incompletely allocated closure
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0" && getenv("USE_TRACKED_ALLOC") !== "1") {
    die("skip Zend MM disabled");
}
?>
--FILE--
<?php

function test() {
	$k = 1;
	return function () use ($k) {
		foo();
	};
}

ini_set('memory_limit', '2M');

$array = [];
for ($i = 0; $i < 10_000; $i++) {
	$array[] = test();
}

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
