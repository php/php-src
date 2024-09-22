--TEST--
Bug #54268 (Double free when destroy_zend_class fails)
--INI--
memory_limit=8M
--SKIPIF--
<?php
$zend_mm_enabled = getenv("USE_ZEND_ALLOC");
if ($zend_mm_enabled === "0") {
    die("skip Zend MM disabled");
}
$tracing = extension_loaded("Zend OPcache")
    && ($conf = opcache_get_configuration()["directives"])
    && array_key_exists("opcache.jit", $conf)
    &&  $conf["opcache.jit"] === "tracing";
if (PHP_OS_FAMILY === "Windows" && PHP_INT_SIZE == 8 && $tracing) {
    $url = "https://github.com/php/php-src/issues/15709";
    die("xfail Test fails on Windows x64 (VS17) and tracing JIT; see $url");
}
?>
--FILE--
<?php
class DestructableObject
{
        public function __destruct()
        {
                DestructableObject::__destruct();
        }
}
class DestructorCreator
{
        public $test;
        public function __destruct()
        {
                $this->test = new DestructableObject;
        }
}
class Test
{
        public static $mystatic;
}
$x = new Test();
Test::$mystatic = new DestructorCreator();
?>
--EXPECTF--
Fatal error: Allowed memory size of %s bytes exhausted%s(tried to allocate %s bytes) in %s on line %d
