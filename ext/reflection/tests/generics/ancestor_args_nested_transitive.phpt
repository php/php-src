--TEST--
Reflection: transitive parent interface arguments substitute nested type parameters
--FILE--
<?php
class Box<T> {}
interface Root<T> {}
interface PairRoot<A, B> {}
interface Mid<X> extends Root<Box<X>> {}
interface Flip<A, B> extends PairRoot<Box<B>, Box<A>> {}

class Concrete implements Mid<int> {}
class Forwarded<U> implements Mid<U> {}
class Reordered<X, Y> implements Flip<X, Y> {}

function show(string $class, string $interface): void {
    $bindings = (new ReflectionClass($class))->getGenericArgumentsForParentInterface($interface);
    echo "$class/$interface (", count($bindings), " binding)\n";
    foreach ($bindings as $args) {
        foreach ($args as $arg) {
            echo "  ", $arg->getName();
            if ($arg instanceof ReflectionNamedType && $arg->hasGenericArguments()) {
                echo "<", implode(", ", array_map(
                    static fn(ReflectionType $type): string => $type->getName(),
                    $arg->getGenericArguments(),
                )), ">";
            }
            echo "\n";
        }
    }
}

show(Concrete::class, Root::class);
show(Forwarded::class, Root::class);
show(Reordered::class, PairRoot::class);

$forwardedBindings = (new ReflectionClass(Forwarded::class))->getGenericArgumentsForParentInterface(Root::class);
$forwardedInner = $forwardedBindings[0][0]->getGenericArguments()[0];
echo "Forwarded nested parameter owner: ",
    $forwardedInner->getTypeParameter()->getDeclaringEntity()->getName(), "\n";
?>
--EXPECT--
Concrete/Root (1 binding)
  Box<int>
Forwarded/Root (1 binding)
  Box<U>
Reordered/PairRoot (1 binding)
  Box<Y>
  Box<X>
Forwarded nested parameter owner: Forwarded
