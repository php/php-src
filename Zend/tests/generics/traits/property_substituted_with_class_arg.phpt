--TEST--
Traits: trait property substituted with a class type
--FILE--
<?php
class Animal {}

trait Holder<T> {
    public T $val;
}

class AnimalBox { use Holder<Animal>; }

$rp = (new ReflectionClass('AnimalBox'))->getProperty('val');
echo $rp->getType()->getName(), "\n";

$b = new AnimalBox;
$b->val = new Animal;
echo get_class($b->val), "\n";

try {
    $b->val = 5;
} catch (TypeError) {
    echo "TypeError\n";
}
?>
--EXPECT--
Animal
Animal
TypeError
