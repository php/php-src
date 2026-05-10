--TEST--
Inherited methods: variadic parameter typed T is substituted on the child class
--FILE--
<?php
class Base<T> {
    public function all(T ...$xs): int { return count($xs); }
}

class IntBase extends Base<int> {}

$rm = (new ReflectionClass('IntBase'))->getMethod('all');
echo "param: ", $rm->getParameters()[0]->getType()->getName(), "\n";
?>
--EXPECT--
param: int
