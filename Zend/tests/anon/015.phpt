--TEST--
static variables in methods inherited from parent class
--FILE--
<?php
class C {
    function foo ($y = null) {
        static $x = null;
        if (!is_null($y)) {
            $x = [$y];
        }
        return $x;
    }
}
$c = new C();
$c->foo(42);
$d = new class extends C {};
var_dump($d->foo());
var_dump($d->foo(24));
var_dump($c->foo());
?>
--EXPECT--
array(1) {
  [0]=>
  int(42)
}
array(1) {
  [0]=>
  int(24)
}
array(1) {
  [0]=>
  int(24)
}
