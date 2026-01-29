--TEST--
(unset) cast must not be allowed in constant property initializer
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
--EXPECT--
Invalid cast in constant expression
