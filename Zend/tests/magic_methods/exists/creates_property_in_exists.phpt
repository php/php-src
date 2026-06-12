--TEST--
__exists: when __exists creates the property, __get is not called (fixes GH-12695)
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public function __exists(string $n): bool {
        echo "  __exists($n)\n";
        $this->$n = 123;
        return true;
    }
    public function __get(string $n): mixed {
        throw new Exception("__get must not be called when __exists materialized the property");
    }
}

echo "1) `??` when __exists creates the property: __get is skipped:\n";
$c = new C;
var_dump($c->foo ?? 'fallback');

echo "\n2) Subsequent `??` reads the now-real property without any magic call:\n";
var_dump($c->foo ?? 'fallback');

echo "\n3) isset() when __exists creates the property: __get is skipped (real prop, value 123 is non-null):\n";
$c = new C;
var_dump(isset($c->foo));

echo "\n4) When __exists materialises the property to null, `??` falls back:\n";
#[AllowDynamicProperties]
class D {
    public function __exists(string $n): bool {
        echo "  __exists($n)\n";
        $this->$n = null;
        return true;
    }
    public function __get(string $n): mixed {
        throw new Exception("__get must not be called when __exists materialised the property");
    }
}
$d = new D;
var_dump($d->foo ?? 'fallback');

?>
--EXPECT--
1) `??` when __exists creates the property: __get is skipped:
  __exists(foo)
int(123)

2) Subsequent `??` reads the now-real property without any magic call:
int(123)

3) isset() when __exists creates the property: __get is skipped (real prop, value 123 is non-null):
  __exists(foo)
bool(true)

4) When __exists materialises the property to null, `??` falls back:
  __exists(foo)
string(8) "fallback"
