--TEST--
Compound assignment to a typed property through read_property/write_property returns the uncoerced result
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
final class B {
    public bool $b = false;
    function __construct() { unset($this->b); }
    function __get($n) { return 100; }
}

class H {
    public int $x {
        set => $value;
    }
}

class P {
    public bool $b = false;
}

class Q extends P {
    function __construct() { unset($this->b); }
    function __get($n) { return 100; }
}

final class R {
    public readonly int $p;
    function __construct() { $this->p = 1; }
    function __clone() {
        $r = ($this->p += 1.5);
        var_dump([is_int($r), $r]);
    }
}

function f(B $o) {
    $r = ($o->b += 1);
    return [is_bool($r), $r];
}

function g(H $o) {
    $o->x = 1;
    $r = ($o->x += 1.5);
    return [is_int($r), $r];
}

function h(P $o) {
    $r = ($o->b += 1);
    return [is_bool($r), $r];
}

var_dump(f(new B));
var_dump(g(new H));
var_dump(h(new Q));
clone new R;
?>
--EXPECTF--
array(2) {
  [0]=>
  bool(false)
  [1]=>
  int(2)
}

Deprecated: Implicit conversion from float 2.5 to int loses precision in %s on line %d
array(2) {
  [0]=>
  bool(false)
  [1]=>
  float(2.5)
}
array(2) {
  [0]=>
  bool(false)
  [1]=>
  int(2)
}

Deprecated: Implicit conversion from float 2.5 to int loses precision in %s on line %d
array(2) {
  [0]=>
  bool(false)
  [1]=>
  float(2.5)
}
