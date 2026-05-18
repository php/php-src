--TEST--
Reflection: hasGenericArguments and getGenericArguments
--FILE--
<?php
class Box<T> {}
function f(Box<int> $x): Box<string> { return $x; }
$r = new ReflectionFunction('f');
$pt = $r->getParameters()[0]->getType();
var_dump($pt->hasGenericArguments());
$args = $pt->getGenericArguments();
echo count($args), "\n";
echo $args[0]->getName(), "\n";

$rt = $r->getReturnType();
var_dump($rt->hasGenericArguments());
$args2 = $rt->getGenericArguments();
echo $args2[0]->getName(), "\n";
?>
--EXPECT--
bool(true)
1
int
bool(true)
string
