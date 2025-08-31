--TEST--
static type in trait
--FILE--
<?php

trait T {
    public function test($arg): static {
        return $arg;
    }
}

class C {
    use T;
}
class P extends C {
}

$c = new C;
$p = new P;
var_dump($c->test($c));
var_dump($c->test($p));
var_dump($p->test($p));
var_dump($p->test($c));

?>
--EXPECTF--
object(C)#1 (0) {
}
object(P)#2 (0) {
}
object(P)#2 (0) {
}

Fatal error: Uncaught TypeError: C::test(): Return value must be of type P, C returned in %s:%d
Stack trace:
#0 %s(%d): C->test(Object(C))
#1 {main}
  thrown in %s on line %d
