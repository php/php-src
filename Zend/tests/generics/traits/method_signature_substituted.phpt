--TEST--
Traits: trait method param and return types are substituted from the use clause's bound arguments
--FILE--
<?php
trait Holder<T> {
    public function take(T $x): T { return $x; }
}

class IntHolder { use Holder<int>; }
class StrHolder { use Holder<string>; }

foreach (['IntHolder', 'StrHolder'] as $cn) {
    $rm = (new ReflectionClass($cn))->getMethod('take');
    $p = $rm->getParameters()[0]->getType()->getName();
    $r = $rm->getReturnType()->getName();
    echo "$cn::take: $p -> $r\n";
}

$ih = new IntHolder;
echo $ih->take(7), "\n";
?>
--EXPECT--
IntHolder::take: int -> int
StrHolder::take: string -> string
7
