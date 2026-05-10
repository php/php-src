--TEST--
Property substitution: set hook value parameter type is substituted on a class extending a generic parent
--FILE--
<?php
class Box<T> {
    public T $val { set => $this->val = $value; }
    public function __construct(T $v) { $this->val = $v; }
}

class IntBox extends Box<int> {}

$rp = (new ReflectionClass('IntBox'))->getProperty('val');
echo "prop type: ", $rp->getType()->getName(), "\n";

$set = $rp->getHook(PropertyHookType::Set);
echo "set param: ", $set->getParameters()[0]->getType()->getName(), "\n";

$ib = new IntBox(5);
$ib->val = 10;
echo $ib->val, "\n";

try {
    $ib->val = "no";
} catch (TypeError) {
    echo "TypeError\n";
}
?>
--EXPECT--
prop type: int
set param: int
10
TypeError
