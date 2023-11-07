--TEST--
Type inference verification 005
--FILE--
<?php

class C {
    private array $prop;

    public function w() {
        $this->prop->prop2 = 42;
    }

    public function unset() {
        unset($this->prop['foo']);
    }
}

$c = new C();
try {
    $c->w('foo');
} catch (Error $e) {}
$c->unset('foo');

?>
===DONE===
--EXPECT--
===DONE===
