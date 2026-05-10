--TEST--
Diamond + trait-use: only T-touching methods are merged; non-generic methods are unaffected
--FILE--
<?php
trait Box<in X> {
    public function set(X $v): void { echo "set ", var_export($v, true), "\n"; }
    public function ping(): string { return "pong"; }
}

class C {
    use Box<int>, Box<string>;
}

$r = new ReflectionClass(C::class);
echo "set param:  ", (string)$r->getMethod('set')->getParameters()[0]->getType(), "\n";
echo "ping return: ", (string)$r->getMethod('ping')->getReturnType(), "\n";

$c = new C;
$c->set(1);
$c->set("hi");
echo $c->ping(), "\n";
?>
--EXPECT--
set param:  string|int
ping return: string
set 1
set 'hi'
pong
