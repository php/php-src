--TEST--
GH-14639 (Member access within null pointer in ext/spl/spl_observer.c)
--INI--
memory_limit=2M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") {
    die("skip Zend MM disabled");
}
$tracing = extension_loaded("Zend OPcache")
    && ($conf = opcache_get_configuration()["directives"])
    && array_key_exists("opcache.jit", $conf)
    &&  $conf["opcache.jit"] === "tracing";
if (PHP_OS_FAMILY === "Windows" && PHP_INT_SIZE == 8 && $tracing) {
    $url = "https://github.com/php/php-src/pull/14919#issuecomment-2259003979";
    die("xfail Test fails on Windows x64 (VS17) and tracing JIT; see $url");
}
?>
--FILE--
<?php
$b = new SplObjectStorage();
for ($i = 10000; $i > 0; $i--) {
    $object = new StdClass();
    $object->a = str_repeat("a", 2);
    $b->attach($object);
}
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
