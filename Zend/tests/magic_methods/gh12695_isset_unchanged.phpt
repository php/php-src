--TEST--
GH-12695: isset() itself is unchanged (still does not consult __get)
--FILE--
<?php

/* The GH-12695 fix only affects `??` and `empty()` (which currently call
 * __get after __isset). The isset() construct itself has never consulted
 * __get and continues not to: it returns whatever __isset returned, cast
 * to bool. */

class C {
    public function __get($n) {
        throw new Exception("__get must not be called from a plain isset()");
    }
    public function __isset($n) {
        echo "  __isset($n)\n";
        return true;
    }
}
$c = new C;
var_dump(isset($c->any));

?>
--EXPECT--
  __isset(any)
bool(true)
