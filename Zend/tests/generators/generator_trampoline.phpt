--TEST--
Calling generator through magic __call()
--FILE--
<?php
class A {
    public function __call($name, $args) {
        for ($i = 0; $i < 5; $i++) {
            yield $i;
        }
    }
}

$a = new A();
foreach ($a->gen() as $n) {
    var_dump($n);
}
$a->gen();
?>
--EXPECT--
int(0)
int(1)
int(2)
int(3)
int(4)
