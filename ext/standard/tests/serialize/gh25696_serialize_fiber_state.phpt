--TEST--
GH-25696: serialize state survives a Fiber suspended during nested serialization
--FILE--
<?php
error_reporting(E_ALL & ~E_DEPRECATED);

class Inner {
    public function __serialize(): array {
        Fiber::suspend();
        return ['x' => 1];
    }
}

class Outer implements Serializable {
    public $fiber;

    public function serialize(): string {
        $this->fiber->start();
        return 'x';
    }

    public function unserialize($data): void {}
}

$inner = new Inner;
$fiber = new Fiber(function () use ($inner) {
    serialize($inner);
});
$outer = new Outer;
$outer->fiber = $fiber;

serialize($outer);
$fiber->resume();
var_dump(serialize(1));
?>
--EXPECT--
string(4) "i:1;"
