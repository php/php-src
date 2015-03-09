--TEST--
Pass uninitialised variables by reference and by value to test implicit initialisation.
--FILE--
<?php

function v($val) {
  $val = "Val changed";
}

function r(&$ref) {
  $ref = "Ref changed";
}


function vv($val1, $val2) {
  $val1 = "Val1 changed";
  $val2 = "Val2 changed";
}

function vr($val, &$ref) {
  $val = "Val changed";
  $ref = "Ref changed";
}

function rv(&$ref, $val) {
  $val = "Val changed";
  $ref = "Ref changed";
}

function rr(&$ref1, &$ref2) {
  $ref1 = "Ref1 changed";
  $ref2 = "Ref2 changed";
}


class C {

	function __construct($val, &$ref) {
	  $val = "Val changed";
	  $ref = "Ref changed";
	}

	function v($val) {
	  $val = "Val changed";
	}
	
	function r(&$ref) {
	  $ref = "Ref changed";
	}
	
	function vv($val1, $val2) {
	  $val1 = "Val1 changed";
	  $val2 = "Val2 changed";
	}
	
	function vr($val, &$ref) {
	  $val = "Val changed";
	  $ref = "Ref changed";
	}
	
	function rv(&$ref, $val) {
	  $val = "Val changed";
	  $ref = "Ref changed";
	}
	
	function rr(&$ref1, &$ref2) {
	  $ref1 = "Ref1 changed";
	  $ref2 = "Ref2 changed";
	}

} 

echo "\n ---- Pass by ref / pass by val: functions ----\n";
unset($u1, $u2);
v($u1);
r($u2);
var_dump($u1, $u2);

unset($u1, $u2);
vv($u1, $u2);
var_dump($u1, $u2);

unset($u1, $u2);
vr($u1, $u2);
var_dump($u1, $u2);

unset($u1, $u2);
rv($u1, $u2);
var_dump($u1, $u2);

unset($u1, $u2);
rr($u1, $u2);
var_dump($u1, $u2);


echo "\n\n ---- Pass by ref / pass by val: static method calls ----\n";
unset($u1, $u2);
C::v($u1);
C::r($u2);
var_dump($u1, $u2);

unset($u1, $u2);
C::vv($u1, $u2);
var_dump($u1, $u2);

unset($u1, $u2);
C::vr($u1, $u2);
var_dump($u1, $u2);

unset($u1, $u2);
C::rv($u1, $u2);
var_dump($u1, $u2);

unset($u1, $u2);
C::rr($u1, $u2);
var_dump($u1, $u2);

echo "\n\n ---- Pass by ref / pass by val: instance method calls ----\n";
unset($u1, $u2);
$c = new C($u1, $u2);
var_dump($u1, $u2);

unset($u1, $u2);
$c->v($u1);
$c->r($u2);
var_dump($u1, $u2);

unset($u1, $u2);
$c->vv($u1, $u2);
var_dump($u1, $u2);

unset($u1, $u2);
$c->vr($u1, $u2);
var_dump($u1, $u2);

unset($u1, $u2);
$c->rv($u1, $u2);
var_dump($u1, $u2);

unset($u1, $u2);
$c->rr($u1, $u2);
var_dump($u1, $u2);

?>
--EXPECTF--

---- Pass by ref / pass by val: functions ----

Notice: Undefined variable: u1 in %s on line 72

Notice: Undefined variable: u1 in %s on line 74
NULL
string(11) "Ref changed"

Notice: Undefined variable: u1 in %s on line 77

Notice: Undefined variable: u2 in %s on line 77

Notice: Undefined variable: u1 in %s on line 78

Notice: Undefined variable: u2 in %s on line 78
NULL
NULL

Notice: Undefined variable: u1 in %s on line 81

Notice: Undefined variable: u1 in %s on line 82
NULL
string(11) "Ref changed"

Notice: Undefined variable: u2 in %s on line 85

Notice: Undefined variable: u2 in %s on line 86
string(11) "Ref changed"
NULL
string(12) "Ref1 changed"
string(12) "Ref2 changed"


 ---- Pass by ref / pass by val: static method calls ----

Strict Standards: Non-static method C::v() should not be called statically in %s on line 95

Notice: Undefined variable: u1 in %s on line 95

Strict Standards: Non-static method C::r() should not be called statically in %s on line 96

Notice: Undefined variable: u1 in %s on line 97
NULL
string(11) "Ref changed"

Strict Standards: Non-static method C::vv() should not be called statically in %s on line 100

Notice: Undefined variable: u1 in %s on line 100

Notice: Undefined variable: u2 in %s on line 100

Notice: Undefined variable: u1 in %s on line 101

Notice: Undefined variable: u2 in %s on line 101
NULL
NULL

Strict Standards: Non-static method C::vr() should not be called statically in %s on line 104

Notice: Undefined variable: u1 in %s on line 104

Notice: Undefined variable: u1 in %s on line 105
NULL
string(11) "Ref changed"

Strict Standards: Non-static method C::rv() should not be called statically in %s on line 108

Notice: Undefined variable: u2 in %s on line 108

Notice: Undefined variable: u2 in %s on line 109
string(11) "Ref changed"
NULL

Strict Standards: Non-static method C::rr() should not be called statically in %s on line 112
string(12) "Ref1 changed"
string(12) "Ref2 changed"


 ---- Pass by ref / pass by val: instance method calls ----

Notice: Undefined variable: u1 in %s on line 117

Notice: Undefined variable: u1 in %s on line 118
NULL
string(11) "Ref changed"

Notice: Undefined variable: u1 in %s on line 121

Notice: Undefined variable: u1 in %s on line 123
NULL
string(11) "Ref changed"

Notice: Undefined variable: u1 in %s on line 126

Notice: Undefined variable: u2 in %s on line 126

Notice: Undefined variable: u1 in %s on line 127

Notice: Undefined variable: u2 in %s on line 127
NULL
NULL

Notice: Undefined variable: u1 in %s on line 130

Notice: Undefined variable: u1 in %s on line 131
NULL
string(11) "Ref changed"

Notice: Undefined variable: u2 in %s on line 134

Notice: Undefined variable: u2 in %s on line 135
string(11) "Ref changed"
NULL
string(12) "Ref1 changed"
string(12) "Ref2 changed"
