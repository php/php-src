--TEST--
GH-20854 (Assertion in ZEND_RETURN_BY_REF with lazy proxy and return-by-ref __get)
--FILE--
<?php
class C {
    public $prop;

    function &__get($name) {
        return $this->x;
    }
}

$rc = new ReflectionClass(C::class);
$obj = $rc->newLazyProxy(function () {
    return new C;
});
$obj->x;
echo "Done\n";
?>
--EXPECTF--
Deprecated: Creation of dynamic property C::$x is deprecated in %s on line %d
Done
