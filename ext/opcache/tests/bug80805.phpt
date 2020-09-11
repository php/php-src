--TEST--
Bug #80805: create simple class and get error in opcache.so
--FILE--
<?php

class Test {
    public int $foo;
    public function __construct()
    {
        $this->foo = 2;
    }
    public function inc(): int
    {
        return $this->foo += 2;
    }
}

$test = new Test();
var_dump($test->foo);
$test->inc();
var_dump($test->foo);

?>
--EXPECT--
int(2)
int(4)
