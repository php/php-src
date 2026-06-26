--TEST--
Generic syntax: two levels of nesting (>> splitting)
--FILE--
<?php
class Outer {}
class Inner {}
function f(): Outer<Inner<int>> { return new Outer; }
$rt = (new ReflectionFunction('f'))->getReturnType();
echo $rt->getName(), "\n";
$arg = $rt->getGenericArguments()[0];
echo $arg->getName(), "\n";
echo $arg->getGenericArguments()[0]->getName(), "\n";
?>
--EXPECT--
Outer
Inner
int
