--TEST--
Short methods.
--FILE--
<?php

class Test {

    public function __construct(private int $b) {}

    public function addUp(int $a) => $a + $this->b;

    public function addUp2(int $a)
        => $a + $this->b;
}

$t = new Test(1);

print $t->addUp(5) . PHP_EOL;
print $t->addUp2(5) . PHP_EOL;

?>
--EXPECT--
6
6
