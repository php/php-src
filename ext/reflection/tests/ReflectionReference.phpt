--TEST--
Basic ReflectionReference functionality
--FILE--
<?php

$ary = [0, 1, 2, 3];
$ref1 =& $ary[1];
unset($ref1);
$ref2 =& $ary[2];
$ref3 =& $ary[3];

echo "fromArrayElement():\n";
$r0 = ReflectionReference::fromArrayElement($ary, 0);
var_dump($r0 === null);
$r1 = ReflectionReference::fromArrayElement($ary, 1);
var_dump($r1 === null);
$r2 = ReflectionReference::fromArrayElement($ary, 2);
var_dump($r2 instanceof ReflectionReference);
$r3 = ReflectionReference::fromArrayElement($ary, 3);
var_dump($r2 instanceof ReflectionReference);

echo "getId() #1:\n";
var_dump($r2->getId() === $r2->getId());
var_dump($r3->getId() === $r3->getId());
var_dump($r2->getId() !== $r3->getId());

echo "getId() #2:\n";
$ary2 = [&$ary[2], &$ref3];
$r2_2 = ReflectionReference::fromArrayElement($ary2, 0);
$r3_2 = ReflectionReference::fromArrayElement($ary2, 1);
var_dump($r2->getId() === $r2_2->getId());
var_dump($r3->getId() === $r3_2->getId());

echo "getId() #3:\n";
$r2_id = $r2->getId();
$r3_id = $r3->getId();
unset($r0, $r1, $r2, $r3, $r2_2, $r3_2);
$r2 = ReflectionReference::fromArrayElement($ary, 2);
$r3 = ReflectionReference::fromArrayElement($ary, 3);
var_dump($r2_id === $r2->getId());
var_dump($r3_id === $r3->getId());

?>
--EXPECT--
fromArrayElement():
bool(true)
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
