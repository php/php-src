--TEST--
Asymmetric visibility: cache slot primed by a __set() fallback must not skip the set visibility check
--FILE--
<?php
class C {
    public private(set) int $x = 0;
    public function __set($name, $value) { echo "__set($name)\n"; }
    public function unsetX() { unset($this->x); }
}
function w(C $c, int $v) {
    try {
        $c->x = $v;   // same opline every call
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
$a = new C();
$a->unsetX();
w($a, 1);         // unset property: goes to __set(), cache slot stays primed
$b = new C();
w($b, 2);         // initialized property, must fail: private(set) from global scope
var_dump($b->x);

?>
--EXPECT--
__set(x)
Cannot modify private(set) property C::$x from global scope
int(0)
