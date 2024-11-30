--TEST--
GH-12073: Freeing of non-ZMM pointer of incompletely allocated closure
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0" && getenv("USE_TRACKED_ALLOC") !== "1") {
    die("skip Zend MM disabled");
}
$tracing = extension_loaded("Zend OPcache")
    && ($conf = opcache_get_configuration()["directives"])
    && array_key_exists("opcache.jit", $conf)
    &&  $conf["opcache.jit"] === "tracing";
if (PHP_OS_FAMILY === "Windows" && PHP_INT_SIZE == 8 && $tracing && getenv('SKIP_ASAN')) {
    $url = "https://github.com/php/php-src/issues/15709";
    die("xfail Test fails on Windows x64 (VS17) and tracing JIT with ASan; see $url");
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
