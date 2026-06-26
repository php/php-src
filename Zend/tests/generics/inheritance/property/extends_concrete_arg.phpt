--TEST--
Property substitution: class extends generic parent with concrete arg substitutes property type
--FILE--
<?php
class Box<T> {
    public T $val;
}

class IntBox extends Box<int> {}

$rp = (new ReflectionClass('IntBox'))->getProperty('val');
echo $rp->getType()->getName(), "\n";

$ib = new IntBox;
$ib->val = 42;
echo $ib->val, "\n";

try {
    $ib->val = [];
} catch (TypeError) {
    echo "TypeError\n";
}
?>
--EXPECT--
int
42
TypeError
