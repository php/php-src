--TEST--
Generic syntax: turbofish on first-class callable creation
--FILE--
<?php
function f<T>(int $x): int { return $x * 2; }
class C {
    public static function s<T>(int $x): int { return $x + 100; }
    public function m<T>(int $x): int { return $x - 1; }
}

$f = f::<int>(...);
echo $f(5), "\n";

$g = C::s::<int>(...);
echo $g(5), "\n";

$c = new C;
$h = $c->m::<int>(...);
echo $h(5), "\n";
?>
--EXPECT--
10
105
4
