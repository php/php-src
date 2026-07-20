--TEST--
GH-12695: __get() invocation is based on __isset()'s return value
--FILE--
<?php

/* The re-check after __isset() must not affect the legacy path when
 * the property is genuinely magic-only: __get() is still called and
 * its return value drives `??`'s null check. */

echo "`??` when __isset=true and __get returns a value: __get is called:\n";
class C {
    public function __get($n) {
        echo "  __get($n)\n";
        return 'from-get';
    }
    public function __isset($n) {
        echo "  __isset($n)\n";
        return true;
    }
}
$c = new C;
var_dump($c->any ?? 'fallback');

echo "\n`??` when __isset=true and __get returns null: __get is called and fallback is used:\n";
class D {
    public function __get($n) {
        echo "  __get($n)\n";
        return null;
    }
    public function __isset($n) {
        echo "  __isset($n)\n";
        return true;
    }
}
$d = new D;
var_dump($d->any ?? 'fallback');

echo "\n`??` when __isset returns false: __get is not called:\n";
class E {
    public function __get($n) {
        throw new Exception("__get must not be called when __isset returned false");
    }
    public function __isset($n) {
        echo "  __isset($n)\n";
        return false;
    }
}
$e = new E;
var_dump($e->any ?? 'fallback');

?>
--EXPECT--
`??` when __isset=true and __get returns a value: __get is called:
  __isset(any)
  __get(any)
string(8) "from-get"

`??` when __isset=true and __get returns null: __get is called and fallback is used:
  __isset(any)
  __get(any)
string(8) "fallback"

`??` when __isset returns false: __get is not called:
  __isset(any)
string(8) "fallback"
