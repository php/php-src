--TEST--
Property substitution: get hook return type substitutes through a generic intermediate class
--FILE--
<?php
class Box<T> {
    public T $val { get => $this->val; }
    public function __construct(T $v) { $this->val = $v; }
}

class Mid<U> extends Box<U> {}
class Leaf extends Mid<float> {}

$rp = (new ReflectionClass('Leaf'))->getProperty('val');
echo "prop type: ", $rp->getType()->getName(), "\n";

$get = $rp->getHook(PropertyHookType::Get);
echo "get return: ", $get->getReturnType()->getName(), "\n";

$leaf = new Leaf(3.14);
echo "value: ", $leaf->val, "\n";
?>
--EXPECT--
prop type: float
get return: float
value: 3.14
