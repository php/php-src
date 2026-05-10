--TEST--
Property substitution: get hook return type is substituted on a class extending a generic parent
--FILE--
<?php
class Box<T> {
    public T $val { get => $this->val; }
    public function __construct(T $v) { $this->val = $v; }
}

class IntBox extends Box<int> {}

$rp = (new ReflectionClass('IntBox'))->getProperty('val');
echo "prop type: ", $rp->getType()->getName(), "\n";

$get = $rp->getHook(PropertyHookType::Get);
echo "get return: ", $get->getReturnType()->getName(), "\n";

$ib = new IntBox(42);
echo "value: ", $ib->val, "\n";
?>
--EXPECT--
prop type: int
get return: int
value: 42
