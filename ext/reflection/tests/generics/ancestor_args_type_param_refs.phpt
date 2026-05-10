--TEST--
Reflection: type-parameter references inside extends/implements args resolve correctly
--FILE--
<?php
interface Container<X> {}
class Holder<T : object> implements Container<T> {}

$rc = new ReflectionClass('Holder');
$bindings = $rc->getGenericArgumentsForParentInterface('Container');

echo "bindings: ", count($bindings), "\n";
echo "args in binding 0: ", count($bindings[0]), "\n";
echo "class: ", get_class($bindings[0][0]), "\n";
echo "name: ", $bindings[0][0]->getName(), "\n";

$param = $bindings[0][0]->getTypeParameter();
echo "type param: ", $param->getName(), "\n";
echo "declaring: ", $param->getDeclaringEntity()->getName(), "\n";
?>
--EXPECT--
bindings: 1
args in binding 0: 1
class: ReflectionTypeParameterReference
name: T
type param: T
declaring: Holder
