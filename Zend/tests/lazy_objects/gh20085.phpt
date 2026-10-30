--TEST--
GH-20085 (Assertion failure when combining lazy object get_properties exception with foreach loop)
--FILE--
<?php
class C {
    public int $a;
    public function __construct() {
        $this->a = 1;
    }
}
$obj = new C;
$reflector = new ReflectionClass(C::class);
foreach ($obj as &$value) {
    $obj = $reflector->newLazyGhost(function ($obj) {
        throw new Error;
    });
}
echo !obj;
?>
--EXPECTF--
Fatal error: Uncaught Error in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(Object(C))
#1 {main}
  thrown in %s on line %d
