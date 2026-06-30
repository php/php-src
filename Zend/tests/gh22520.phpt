--TEST--
GH-22520: Lazy object OOM should not crash zend_lazy_object_del_info
--SKIPIF--
<?php
if (getenv('SKIP_SLOW_TESTS')) die("skip slow test");
?>
--INI--
memory_limit=2M
--FILE--
<?php

class C {
    public $a;
}

$str = str_repeat('a', 1024 * 1024 * 1.25);

try {
    $reflector = new ReflectionClass(C::class);
    for ($i = 0; $i < 10000; $i++) {
        $obj = $reflector->newLazyGhost(function ($obj) {});
        $reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, $obj);
    }
} catch (Throwable $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted at %s:%d (tried to allocate %d bytes) in %s on line %d
