--TEST--
Traits: concretely typed parameters and returns are unaffected by substitution
--FILE--
<?php
trait Holder<T> {
    public T $val;
    public function takeT(T $x): T { return $x; }
    public function bare(int $x): string { return (string)$x; }
}

class StrHolder { use Holder<string>; }

$rm = (new ReflectionClass('StrHolder'))->getMethod('bare');
echo "bare: ",
    $rm->getParameters()[0]->getType()->getName(), " -> ",
    $rm->getReturnType()->getName(), "\n";

$rm = (new ReflectionClass('StrHolder'))->getMethod('takeT');
echo "takeT: ",
    $rm->getParameters()[0]->getType()->getName(), " -> ",
    $rm->getReturnType()->getName(), "\n";
?>
--EXPECT--
bare: int -> string
takeT: string -> string
