--TEST--
Generic syntax: in/out variance markers are contextual and never reserved
--FILE--
<?php
function in(): string { return "fn-in"; }
function out(): string { return "fn-out"; }
const in = "const-in";
const out = "const-out";

class in {}
class out {}

class Holder {
    const in = "member-in";
    public int $out = 7;
    public function in(): string { return "method-in"; }
    public static function out(): string { return "static-out"; }
}

echo in(), "\n";
echo out(), "\n";
echo in, "\n";
echo out, "\n";
echo get_class(new in()), "\n";
echo get_class(new out()), "\n";
echo Holder::in, "\n";
echo (new Holder)->out, "\n";
echo (new Holder)->in(), "\n";
echo Holder::out(), "\n";

interface Producer<out T> { public function get(): T; }
interface Consumer<in T> { public function accept(T $value): void; }

foreach ((new ReflectionClass('Producer'))->getGenericParameters() as $p) {
    echo "Producer ", $p->getName(), ": ", $p->getVariance()->name, "\n";
}
foreach ((new ReflectionClass('Consumer'))->getGenericParameters() as $p) {
    echo "Consumer ", $p->getName(), ": ", $p->getVariance()->name, "\n";
}
?>
--EXPECT--
fn-in
fn-out
const-in
const-out
in
out
member-in
7
method-in
static-out
Producer T: Covariant
Consumer T: Contravariant
