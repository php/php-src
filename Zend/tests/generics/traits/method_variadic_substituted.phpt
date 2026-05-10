--TEST--
Traits: variadic parameter typed T is substituted in the using class
--FILE--
<?php
trait Acceptor<T> {
    public function all(T ...$items): int { return count($items); }
}

class IntAcceptor { use Acceptor<int>; }

$rm = (new ReflectionClass('IntAcceptor'))->getMethod('all');
echo "param type: ", $rm->getParameters()[0]->getType()->getName(), "\n";

$a = new IntAcceptor;
echo $a->all(1, 2, 3), "\n";
?>
--EXPECT--
param type: int
3
