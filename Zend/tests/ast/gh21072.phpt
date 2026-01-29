--TEST--
(unset) cast must not be allowed in constant expressions
--FILE--
<?php
try {
    class C {
        public $p = (unset) C::class;
    }
    new C;
} catch (Error $e) {
    echo $e->getMessage();
}
?>
--EXPECTF--
Fatal error: The (unset) cast is no longer supported in %s on line %d