--TEST--
Generic syntax: self<T>, parent<T>, static<T>
--FILE--
<?php
class Base {}
class Foo extends Base {
    public function s(): self<int> {}
    public function p(): parent<int> {}
    public function st(): static<int> {}
}
$rc = new ReflectionClass('Foo');
foreach (['s' => 'self', 'p' => 'parent', 'st' => 'static'] as $m => $expected) {
    $rt = $rc->getMethod($m)->getReturnType();
    echo "$m: ", $rt->getName(), " hasArgs=", var_export($rt->hasGenericArguments(), true), "\n";
}
?>
--EXPECT--
s: Foo hasArgs=true
p: Base hasArgs=true
st: static hasArgs=true
