--TEST--
scalar type constraint default values
--DESCRIPTION--
Tests that a scalar parameter of a function can have default values of
compatible types.
--FILE--
<?php

function f0(scalar $p = null)  { var_dump($p); }
function f1(scalar $p = true)  { var_dump($p); }
function f2(scalar $p = false) { var_dump($p); }
function f3(scalar $p = 4.2)   { var_dump($p); }
function f4(scalar $p = 42)    { var_dump($p); }
function f5(scalar $p = 'str') { var_dump($p); }

f0();
f1();
f2();
f3();
f4();
f5();

?>
--EXPECT--
NULL
bool(true)
bool(false)
float(4.2)
int(42)
string(3) "str"
