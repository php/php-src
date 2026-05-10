--TEST--
Reflection: getGenericArgumentsForParentClass returns args from extends clause, throws when no parent
--FILE--
<?php
class A<T = mixed> {}
class B<X, Y> {}

class WithArgs extends A<string> {}
class NoArgs extends A {}
class NoParent {}
class Multi extends B<int, float> {}

$cases = [
    'WithArgs' => ['string'],
    'NoArgs'   => [],
    'NoParent' => 'throw',
    'Multi'    => ['int', 'float'],
];

foreach ($cases as $cls => $want) {
    try {
        $args = (new ReflectionClass($cls))->getGenericArgumentsForParentClass();
    } catch (ReflectionException $e) {
        echo $cls, ": ", $want === 'throw' ? "throw OK ({$e->getMessage()})" : "FAIL (unexpected throw)", "\n";
        continue;
    }
    if ($want === 'throw') {
        echo $cls, ": FAIL (expected throw)\n";
    } else {
        $got = array_map(fn($t) => $t->getName(), $args);
        echo $cls, ": ", $got === $want ? "OK" : ("FAIL got " . implode(",", $got)), "\n";
    }
}
?>
--EXPECT--
WithArgs: OK
NoArgs: OK
NoParent: throw OK (Class NoParent has no parent class)
Multi: OK
