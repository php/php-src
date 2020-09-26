--TEST--
Basic short functions return values.
--FILE--
<?php

class Test {

    public function __construct(private int $b) {}

    public function addUp(int $a) => $a + $this->b;
}

$t = new Test(1);

print $t->addUp(5);

?>
--EXPECT--
6
