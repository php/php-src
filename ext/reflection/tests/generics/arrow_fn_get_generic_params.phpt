--TEST--
Reflection: getGenericParameters on arrow function
--FILE--
<?php
$f = fn<T : int>(T $x): T => $x;
$r = new ReflectionFunction($f);
echo $r->isGeneric() ? "gen\n" : "not\n";
echo $r->getGenericParameters()[0]->getName(), "\n";
echo $r->getGenericParameters()[0]->getBound()->getName(), "\n";
?>
--EXPECT--
gen
T
int
