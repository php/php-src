--TEST--
Primary constructors: a class doc comment also applies to the synthesized constructor
--FILE--
<?php
/** Doc for C */
class C(public int $x) {}

$rc = new ReflectionClass(C::class);
var_dump($rc->getDocComment());
var_dump($rc->getConstructor()->getDocComment());

class D(public int $y) {}

$rd = new ReflectionClass(D::class);
var_dump($rd->getDocComment());
var_dump($rd->getConstructor()->getDocComment());

class E(
    /** Doc for z */ public int $z,
) {}

$re = new ReflectionClass(E::class);
var_dump($re->getDocComment());
var_dump($re->getConstructor()->getDocComment());
var_dump($re->getProperty('z')->getDocComment());
?>
--EXPECT--
string(16) "/** Doc for C */"
string(16) "/** Doc for C */"
bool(false)
bool(false)
bool(false)
bool(false)
string(16) "/** Doc for z */"
--CREDITS--
Robert Landers
