--TEST--
Yield can be used during a method call
--FILE--
<?php

class A {
    public function b($c) {
        echo $c, "\n";
    }
}

function gen() {
    $a = new A;
    $a->b(yield);
}

$gen = gen();
$gen->send('foo');

// test resource cleanup
$gen = gen();
$gen->rewind();
unset($gen);

// test cloning
$g1 = gen();
$g1->rewind();
$g2 = clone $g1;
unset($g1);
$g2->send('bar');

?>
--EXPECT--
foo
bar
