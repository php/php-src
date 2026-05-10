--TEST--
Reflection: getGenericArgumentsForParentInterface lists every binding when the same interface is bound more than once
--FILE--
<?php
interface Foo<T = mixed> {}
interface Bar<T> {}
interface Baz<T> extends Bar<T> {}

interface DA<out T> {}
interface I1 extends DA<int> {}
interface I2 extends DA<string> {}

class Box<T> {}

class DirectDup implements Foo<string>, Foo<int> {}

class MixedDup implements Bar<string>, Baz<int> {}

class ParentDup implements Foo<bool>, Foo<float> {}
class ChildDup extends ParentDup {}

class NestedDup implements Foo<Box<int>>, Foo<Box<string>> {}

class DiamondVar implements I1, I2 {}

function render(array $bindings): string {
    return '[' . implode(', ', array_map(
        static fn(array $b): string => '[' . implode(', ', array_map(
            static function (ReflectionType $t): string {
                if ($t instanceof ReflectionNamedType && $t->hasGenericArguments()) {
                    return $t->getName() . '<' . implode(', ', array_map(
                        static fn(ReflectionType $inner): string => $inner->getName(),
                        $t->getGenericArguments(),
                    )) . '>';
                }
                return $t->getName();
            },
            $b,
        )) . ']',
        $bindings,
    )) . ']';
}

function show(string $cls, string $ancestor): void {
    $bindings = (new ReflectionClass($cls))->getGenericArgumentsForParentInterface($ancestor);
    printf("%-12s -> %-5s = %s (count=%d)\n", $cls, $ancestor, render($bindings), count($bindings));
}

show('DirectDup',  'Foo');
show('MixedDup',   'Bar');
show('ChildDup',   'Foo');
show('NestedDup',  'Foo');
show('DiamondVar', 'DA');

class Single implements Foo<int> {}
show('Single', 'Foo');
?>
--EXPECT--
DirectDup    -> Foo   = [[string], [int]] (count=2)
MixedDup     -> Bar   = [[string], [int]] (count=2)
ChildDup     -> Foo   = [[bool], [float]] (count=2)
NestedDup    -> Foo   = [[Box<int>], [Box<string>]] (count=2)
DiamondVar   -> DA    = [[int], [string]] (count=2)
Single       -> Foo   = [[int]] (count=1)
