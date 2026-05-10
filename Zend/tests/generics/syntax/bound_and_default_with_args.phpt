--TEST--
Generic syntax: bound and default both carry type arguments, closing >>
--FILE--
<?php
class B<U> {}

// class A has a generic parameter T with bound B<string> and default B<int>
class A<T:B<string>=B<int>> {}

$p = (new ReflectionClass('A'))->getGenericParameters()[0];

$b = $p->getBound();
echo "bound: ", $b->getName(), "<", $b->getGenericArguments()[0]->getName(), ">\n";

$d = $p->getDefault();
echo "default: ", $d->getName(), "<", $d->getGenericArguments()[0]->getName(), ">\n";
?>
--EXPECT--
bound: B<string>
default: B<int>
