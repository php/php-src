--TEST--
Reflection: getGenericParameters on closure
--FILE--
<?php
$cl = function <T : object>(T $x): T { return $x; };
$r = new ReflectionFunction($cl);
echo $r->isGeneric() ? "gen\n" : "not\n";
echo count($r->getGenericParameters()), "\n";
echo $r->getGenericParameters()[0]->getName(), "\n";
echo $r->getGenericParameters()[0]->getBound()->getName(), "\n";
?>
--EXPECT--
gen
1
T
object
