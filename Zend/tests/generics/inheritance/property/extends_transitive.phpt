--TEST--
Property substitution: substitution composes through a generic intermediate class
--FILE--
<?php
class Box<T> {
    public T $val;
}

class Mid<U> extends Box<U> {}
class Leaf extends Mid<float> {}

$rp = (new ReflectionClass('Leaf'))->getProperty('val');
echo $rp->getType()->getName(), "\n";

$leaf = new Leaf;
$leaf->val = 3.14;
echo $leaf->val, "\n";

try { $leaf->val = "x"; } catch (TypeError) { echo "TypeError\n"; }
?>
--EXPECT--
float
3.14
TypeError
