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

class A {
    public int $x = 1;
}

try {
    $r = new ReflectionClass(A::class);

    // Force lazy ghost creation under low memory pressure
    $obj = $r->newLazyGhost(function () {
        return new A();
    });

    // Try to trigger shutdown GC edge case
    unset($obj);

    echo "OK\n";
} catch (Throwable $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}

?>
--EXPECTF--
OK
