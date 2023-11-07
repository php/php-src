--TEST--
Type inference verification 005
--FILE--
<?php

class C {
    private array $prop;

    public function unset(string $offset)
    {
        unset($this->prop[$offset]);
    }
}

function test() {
    $c = new C();
    $c->unset('foo');
}

test();

?>
===DONE===
--EXPECT--
===DONE===
