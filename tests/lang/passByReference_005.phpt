--TEST--
Pass uninitialized variables by reference and by value to test implicit initialisation.
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

    static function static_v($val) {
        $val = "Val changed";
    }

    static function static_r(&$ref) {
        $ref = "Ref changed";
    }

    static function static_vv($val1, $val2) {
        $val1 = "Val1 changed";
        $val2 = "Val2 changed";
    }

    static function static_vr($val, &$ref) {
        $val = "Val changed";
        $ref = "Ref changed";
    }

    static function static_rv(&$ref, $val) {
        $val = "Val changed";
        $ref = "Ref changed";
    }

    static function static_rr(&$ref1, &$ref2) {
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
C::static_v($u1);
C::static_r($u2);
var_dump($u1, $u2);

unset($u1, $u2);
C::static_vv($u1, $u2);
var_dump($u1, $u2);

unset($u1, $u2);
C::static_vr($u1, $u2);
var_dump($u1, $u2);

unset($u1, $u2);
C::static_rv($u1, $u2);
var_dump($u1, $u2);

unset($u1, $u2);
C::static_rr($u1, $u2);
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

Warning: Undefined variable $u1 in %s on line %d

Warning: Undefined variable $u1 in %s on line %d
NULL
string(11) "Ref changed"

Warning: Undefined variable $u1 in %s on line %d

Warning: Undefined variable $u2 in %s on line %d

Warning: Undefined variable $u1 in %s on line %d

Warning: Undefined variable $u2 in %s on line %d
NULL
NULL

Warning: Undefined variable $u1 in %s on line %d

Warning: Undefined variable $u1 in %s on line %d
NULL
string(11) "Ref changed"

Warning: Undefined variable $u2 in %s on line %d

Warning: Undefined variable $u2 in %s on line %d
string(11) "Ref changed"
NULL
string(12) "Ref1 changed"
string(12) "Ref2 changed"


 ---- Pass by ref / pass by val: static method calls ----

Warning: Undefined variable $u1 in %s on line %d

Warning: Undefined variable $u1 in %s on line %d
NULL
string(11) "Ref changed"

Warning: Undefined variable $u1 in %s on line %d

Warning: Undefined variable $u2 in %s on line %d

Warning: Undefined variable $u1 in %s on line %d

Warning: Undefined variable $u2 in %s on line %d
NULL
NULL

Warning: Undefined variable $u1 in %s on line %d

Warning: Undefined variable $u1 in %s on line %d
NULL
string(11) "Ref changed"

Warning: Undefined variable $u2 in %s on line %d

Warning: Undefined variable $u2 in %s on line %d
string(11) "Ref changed"
NULL
string(12) "Ref1 changed"
string(12) "Ref2 changed"


 ---- Pass by ref / pass by val: instance method calls ----

Warning: Undefined variable $u1 in %s on line %d

Warning: Undefined variable $u1 in %s on line %d
NULL
string(11) "Ref changed"

Warning: Undefined variable $u1 in %s on line %d

Warning: Undefined variable $u1 in %s on line %d
NULL
string(11) "Ref changed"

Warning: Undefined variable $u1 in %s on line %d

Warning: Undefined variable $u2 in %s on line %d

Warning: Undefined variable $u1 in %s on line %d

Warning: Undefined variable $u2 in %s on line %d
NULL
NULL

Warning: Undefined variable $u1 in %s on line %d

Warning: Undefined variable $u1 in %s on line %d
NULL
string(11) "Ref changed"

Warning: Undefined variable $u2 in %s on line %d

Warning: Undefined variable $u2 in %s on line %d
string(11) "Ref changed"
NULL
string(12) "Ref1 changed"
string(12) "Ref2 changed"
