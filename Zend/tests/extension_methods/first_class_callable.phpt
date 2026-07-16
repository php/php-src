--TEST--
First-class callables of extension methods bind the receiver
--FILE--
<?php
class Money {
    public function __construct(public int $cents) {}
}

extension Money $m {
    public function dollars(): float { return $m->cents / 100; }
    public function plus(int $more): float { return ($m->cents + $more) / 100; }
}

$a = new Money(1250);
$f = $a->dollars(...);
var_dump($f());

$g = $a->plus(...);
var_dump($g(50));

/* The callable captured $a's binding, not a live variable. */
$a = new Money(9900);
var_dump($f());
?>
--EXPECT--
float(12.5)
float(13)
float(12.5)
