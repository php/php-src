--TEST--
Const-expr closures inside serialized object graphs
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL)]
class A {
    public function __construct(public mixed $cb = null) {}
}

class Demo {
    #[A(static function () { return 'ok'; })]
    public int $p = 0;
}

class Holder {
    public $c;
    public function __wakeup() {
        // Delayed calls run in creation order: the closure is already
        // initialized when this runs.
        echo "wakeup sees: ", ($this->c)(), "\n";
    }
}

$h = new Holder();
$h->c = (new ReflectionProperty(Demo::class, 'p'))->getAttributes()[0]->getArguments()[0];

$u = unserialize(serialize([$h, $h->c, [$h->c]]));

echo "after: ", ($u[1])(), "\n";
// Shared instances are preserved within the graph.
var_dump($u[0]->c === $u[1], $u[1] === $u[2][0]);

?>
--EXPECT--
wakeup sees: ok
after: ok
bool(true)
bool(true)
