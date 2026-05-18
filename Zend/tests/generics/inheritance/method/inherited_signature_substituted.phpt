--TEST--
Inherited methods: param and return types are substituted on a class extending a generic parent
--FILE--
<?php
class Base<T> {
    public function take(T $x): T { return $x; }
}

class IntBase extends Base<int> {}
class StrBase extends Base<string> {}

foreach (['IntBase', 'StrBase'] as $cn) {
    $rm = (new ReflectionClass($cn))->getMethod('take');
    echo "$cn::take: ",
        $rm->getParameters()[0]->getType()->getName(), " -> ",
        $rm->getReturnType()->getName(), "\n";
}
?>
--EXPECT--
IntBase::take: int -> int
StrBase::take: string -> string
