--TEST--
Bug #47054 (BC break in static functions called as dynamic)
--FILE--
<?php

class C {
  final static function s() {
    print "Called class: " . get_called_class() . "\n";
  }
}
class D extends C {
  public function m() {
    $this->s();
  }
}

$d = new D();
$d->m();

C::s();

$c = new C();
$c->s();

?>
--EXPECT--
Called class: D
Called class: C
Called class: C
