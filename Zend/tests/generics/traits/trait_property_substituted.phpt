--TEST--
Traits: trait property type is substituted from the `use` clause's bound arguments
--FILE--
<?php
trait Holder<T> {
    public T $val;
}

class IntBox { use Holder<int>; }
class StringBox { use Holder<string>; }

$rp = (new ReflectionClass('IntBox'))->getProperty('val');
echo "IntBox::\$val: ", $rp->getType()->getName(), "\n";

$rp = (new ReflectionClass('StringBox'))->getProperty('val');
echo "StringBox::\$val: ", $rp->getType()->getName(), "\n";

$ib = new IntBox;
$ib->val = 7;
echo $ib->val, "\n";

try {
    $ib->val = "x";
} catch (TypeError $e) {
    echo "TypeError\n";
}
?>
--EXPECT--
IntBox::$val: int
StringBox::$val: string
7
TypeError
