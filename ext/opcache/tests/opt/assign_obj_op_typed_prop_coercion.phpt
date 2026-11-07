--TEST--
Type inference of compound assignment to typed properties must account for coercion
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
class B {
    public bool $b = false;
    public int $i = 0;
    public string $s = "1";
    public int|string $is = 0;
    public int|bool $ib = 0;
    public int|false $if = 0;
    public string|bool $sbo = true;
    public float $f = 1.0;
    public static bool $sb = false;
}

function known(B $o) {
    $b = ($o->b += 1);
    $i = ($o->i .= "1");
    $s = ($o->s += 1);
    $sb = (B::$sb += 1);
    return [$b, $i, $s, $sb];
}

function unions(B $o, float $x) {
    $is = ($o->is += $x);
    $ib = ($o->ib += $x);
    $if = ($o->if += 1.0);
    $sbo = ($o->sbo += 1);
    $f = ($o->f |= 2);
    return [is_string($is), $is, is_bool($ib), $ib, $if, is_string($sbo), $sbo, is_float($f), $f];
}

function unknown($o) {
    $r = ($o->b += 1);
    return [$r === true, $r];
}

var_dump(known(new B));
var_dump(unions(new B, INF));
var_dump(unknown(new B));
?>
--EXPECT--
array(4) {
  [0]=>
  bool(true)
  [1]=>
  int(1)
  [2]=>
  string(1) "2"
  [3]=>
  bool(true)
}
array(9) {
  [0]=>
  bool(true)
  [1]=>
  string(3) "INF"
  [2]=>
  bool(true)
  [3]=>
  bool(true)
  [4]=>
  int(1)
  [5]=>
  bool(true)
  [6]=>
  string(1) "2"
  [7]=>
  bool(true)
  [8]=>
  float(3)
}
array(2) {
  [0]=>
  bool(true)
  [1]=>
  bool(true)
}
