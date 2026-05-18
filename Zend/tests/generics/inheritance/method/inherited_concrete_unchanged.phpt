--TEST--
Inherited methods: concretely typed parameters and returns are unaffected by substitution
--FILE--
<?php
class Helper<T> {
    public function bare(int $x): string { return (string)$x; }
}

class IntHelper extends Helper<int> {}

$rm = (new ReflectionClass('IntHelper'))->getMethod('bare');
echo $rm->getParameters()[0]->getType()->getName(), " -> ",
     $rm->getReturnType()->getName(), "\n";
?>
--EXPECT--
int -> string
