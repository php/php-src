--TEST--
Reflection: getGenericArgumentsForParentInterface returns every binding to the ancestor interface; throws when not ancestor
--FILE--
<?php
interface I<K = mixed, V = mixed> {}
interface J<T> {}
interface K1 extends I<int, string> {}
interface Forward<T> extends J<T> {}

class WithArgs implements I<int, string> {}
class WithoutArgs implements I {}
class NotImplements {}
class Multi implements I<bool, float>, J<string> {}
class ViaK1 implements K1 {}
class ViaForward implements Forward<bool> {}
class ChildWithArgs extends WithArgs {}

function render(array $bindings): string {
    return '[' . implode(', ', array_map(
        static fn(array $b): string => '[' . implode(',', array_map(
            static fn(ReflectionType $t): string => $t->getName(),
            $b,
        )) . ']',
        $bindings,
    )) . ']';
}

function show(string $cls, string $iface): void {
    try {
        $bindings = (new ReflectionClass($cls))->getGenericArgumentsForParentInterface($iface);
    } catch (ReflectionException $e) {
        echo "$cls/$iface: throw ({$e->getMessage()})\n";
        return;
    }
    echo "$cls/$iface: ", render($bindings), "\n";
}

show('WithArgs', 'I');
show('WithArgs', 'i'); // case insensitive
show('WithoutArgs', 'I');
show('NotImplements', 'I');
show('Multi', 'I');
show('Multi', 'J');
show('K1', 'I');
show('ViaK1', 'I');
show('ViaForward', 'J');
show('ChildWithArgs', 'I');
?>
--EXPECT--
WithArgs/I: [[int,string]]
WithArgs/i: [[int,string]]
WithoutArgs/I: []
NotImplements/I: throw (I is not an ancestor interface of NotImplements)
Multi/I: [[bool,float]]
Multi/J: [[string]]
K1/I: [[int,string]]
ViaK1/I: [[int,string]]
ViaForward/J: [[bool]]
ChildWithArgs/I: [[int,string]]
