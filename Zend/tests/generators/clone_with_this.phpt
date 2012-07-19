--TEST--
Cloning a generator method (with $this)
--FILE--
<?php

class Test {
    protected $foo;

    public function gen() {
        $this->foo = 'bar';
        yield; // interrupt
        var_dump($this->foo);
    }
}

$g1 = (new Test)->gen();
$g1->rewind(); // goto yield
$g2 = clone $g1;
$g1->close();
$g2->next();

?>
--EXPECT--
string(3) "bar"
