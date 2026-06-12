--TEST--
GH-12695: __get() is not called under `??` when __isset() materialised the property
--FILE--
<?php

#[AllowDynamicProperties]
class A {
    public function __get($n) {
        throw new Exception("__get must not be called when __isset materialised the property");
    }
    public function __isset($n) {
        echo "  __isset($n)\n";
        $this->$n = 123;
        return true;
    }
}

echo "Dynamic property materialised in __isset, then `??`:\n";
$a = new A;
var_dump($a->foo ?? 'fallback');

echo "\nSame on a declared (unset) property:\n";
class B {
    public int $x = 99;
    public function __get($n) {
        throw new Exception("__get must not be called when __isset materialised the property");
    }
    public function __isset($n) {
        echo "  __isset($n)\n";
        $this->$n = 7;
        return true;
    }
}
$b = new B;
unset($b->x);
var_dump($b->x ?? 'fallback');

echo "\nWhen __isset() materialises the property to null, `??` falls back:\n";
#[AllowDynamicProperties]
class D {
    public function __get($n) {
        throw new Exception("__get must not be called when __isset materialised the property");
    }
    public function __isset($n) {
        echo "  __isset($n)\n";
        $this->$n = null;
        return true;
    }
}
$d = new D;
var_dump($d->foo ?? 'fallback');

?>
--EXPECT--
Dynamic property materialised in __isset, then `??`:
  __isset(foo)
int(123)

Same on a declared (unset) property:
  __isset(x)
int(7)

When __isset() materialises the property to null, `??` falls back:
  __isset(foo)
string(8) "fallback"
