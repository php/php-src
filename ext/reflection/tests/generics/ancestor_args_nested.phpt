--TEST--
Reflection: nested generic arguments inside extends/implements/use are preserved
--FILE--
<?php
class Box<T> {}
class Pair<K, V> {}
interface I<X> {}
trait T1<U> { public function noop(): void {} }

class WithNested
    extends Pair<Box<int>, string>
    implements I<Box<float>>
{
    use T1<Box<bool>>;
}

$rc = new ReflectionClass('WithNested');

// Nested in extends
$args = $rc->getGenericArgumentsForParentClass();
echo "parent[0]: ", $args[0]->getName();
echo "<", $args[0]->getGenericArguments()[0]->getName(), ">\n";
echo "parent[1]: ", $args[1]->getName(), "\n";

// Nested in implements
$bindings = $rc->getGenericArgumentsForParentInterface('I');
echo "I bindings: ", count($bindings), "\n";
echo "I[0][0]: ", $bindings[0][0]->getName();
echo "<", $bindings[0][0]->getGenericArguments()[0]->getName(), ">\n";

// Nested in use
$args = $rc->getGenericArgumentsForUsedTrait('T1');
echo "T1[0]: ", $args[0]->getName();
echo "<", $args[0]->getGenericArguments()[0]->getName(), ">\n";
?>
--EXPECT--
parent[0]: Box<int>
parent[1]: string
I bindings: 1
I[0][0]: Box<float>
T1[0]: Box<bool>
