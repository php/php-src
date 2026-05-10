--TEST--
Variance: function-origin type parameters checked uniformly with class-origin
--FILE--
<?php
function take<-T>(T $x): int { return 0; }
function make<+T>(): T { return null; }
function transform<-I, +O>(I $input): O { return null; }

$closure_take = function <-T>(T $x): int { return 0; };
$closure_make = function <+T>(): T { return null; };
$arrow_take   = fn<-T>(T $x): int => 0;
$arrow_make   = fn<+T>(): T => null;

class A<+T> {
    public function map<-A_, +B>(A_ $a): B { return null; }
}

take(42);
make();
transform(1);
$closure_take(7);
$closure_make();
$arrow_take(7);
$arrow_make();
(new A)->map(42);
echo "ok\n";
?>
--EXPECT--
ok
