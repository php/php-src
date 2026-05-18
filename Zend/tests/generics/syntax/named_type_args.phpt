--TEST--
Generic syntax: type arguments on a named type at use site
--FILE--
<?php
class Container {}
function f(Container<int> $x): Container<string> { return $x; }
$r = new ReflectionFunction('f');
$pt = $r->getParameters()[0]->getType();
echo $pt->getName(), "\n";
var_dump($pt->hasGenericArguments());
foreach ($pt->getGenericArguments() as $a) echo $a->getName(), "\n";
?>
--EXPECT--
Container
bool(true)
int
