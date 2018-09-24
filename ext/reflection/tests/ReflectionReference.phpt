--TEST--
Basic ReflectionReference functionality
--FILE--
<?php

$ary = [0, 1, 2];
$ref1 =& $ary[1];
unset($ref1);
$ref2 =& $ary[2];

echo "fromArrayElement():\n";
$r0 = ReflectionReference::fromArrayElement($ary, 0);
var_dump($r0 === null);
$r1 = ReflectionReference::fromArrayElement($ary, 1);
var_dump($r1 instanceof ReflectionReference);
$r2 = ReflectionReference::fromArrayElement($ary, 2);
var_dump($r2 instanceof ReflectionReference);

echo "getId() #1:\n";
var_dump($r1->getId() === $r1->getId());
var_dump($r2->getId() === $r2->getId());
var_dump($r1->getId() !== $r2->getId());

echo "getId() #2:\n";
$ary2 = [&$ary[1], &$ref2];
$r1_2 = ReflectionReference::fromArrayElement($ary2, 0);
$r2_2 = ReflectionReference::fromArrayElement($ary2, 1);
var_dump($r1->getId() === $r1_2->getId());
var_dump($r2->getId() === $r2_2->getId());

echo "getId() #3:\n";
$r1_id = $r1->getId();
$r2_id = $r2->getId();
unset($r0, $r1, $r2, $r1_2, $r2_2);
$r1 = ReflectionReference::fromArrayElement($ary, 1);
$r2 = ReflectionReference::fromArrayElement($ary, 2);
var_dump($r1_id === $r1->getId());
var_dump($r2_id === $r2->getId());

?>
--EXPECT--
fromArrayElement():
bool(true)
bool(true)
bool(true)
getId() #1:
bool(true)
bool(true)
bool(true)
getId() #2:
bool(true)
bool(true)
getId() #3:
bool(true)
bool(true)
