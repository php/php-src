--TEST--
Typed class constants (declaration; compile-type simple)
--FILE--
<?php
class A {
    public const null CONST1 = null;
    public const false CONST2 = false;
    public const true CONST3 = true;
    public const bool CONST4 = true;
    public const int CONST5 = 0;
    public const float CONST6 = 3.14;
    public const float CONST7 = 3;
    public const string CONST8 = "";
    public const array CONST9 = [];
    public const array|string CONST10 = "";
    public const array|string|null CONST11 = null;
}

var_dump(A::CONST1);
var_dump(A::CONST1);
var_dump(A::CONST2);
var_dump(A::CONST2);
var_dump(A::CONST3);
var_dump(A::CONST3);
var_dump(A::CONST4);
var_dump(A::CONST4);
var_dump(A::CONST5);
var_dump(A::CONST5);
var_dump(A::CONST6);
var_dump(A::CONST6);
var_dump(A::CONST7);
var_dump(A::CONST7);
var_dump(A::CONST8);
var_dump(A::CONST8);
var_dump(A::CONST9);
var_dump(A::CONST9);
var_dump(A::CONST10);
var_dump(A::CONST10);
var_dump(A::CONST11);
var_dump(A::CONST11);
?>
--EXPECT--
NULL
NULL
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
int(0)
int(0)
float(3.14)
float(3.14)
float(3)
float(3)
string(0) ""
string(0) ""
array(0) {
}
array(0) {
}
string(0) ""
string(0) ""
NULL
NULL
