--TEST--
Property substitution: extends without args uses parent's parameter default
--FILE--
<?php
class Box<T = int> {
    public T $val;
}

class Bag extends Box {}

$rp = (new ReflectionClass('Bag'))->getProperty('val');
echo $rp->getType()->getName(), "\n";

$b = new Bag;
$b->val = 7;
echo $b->val, "\n";

try { $b->val = "x"; } catch (TypeError) { echo "TypeError\n"; }
?>
--EXPECT--
int
7
TypeError
