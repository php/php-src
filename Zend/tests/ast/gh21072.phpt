--TEST--
(unset) cast must not be allowed in constant expressions
--CREDITS--
Viet Hoang Luu (@vi3tL0u1s)
--FILE--
<?php
try {
    class C {
        public $p = (unset) C::class;
    }
    new C;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
Fatal error: The (unset) cast is no longer supported in %s on line %d
