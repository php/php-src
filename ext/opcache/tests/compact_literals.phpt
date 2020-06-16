--TEST--
Test with compact literals
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

echo "array key hash" . ":" . PHP_EOL;
$array = array(
    "1" => "one",
    "2" => "two",
    "one" => 1,
    "two" => 2,
);

unset($array["one"]);
unset($array["2"]);

print_r($array);

echo "function define" . ":" . PHP_EOL;
if (!function_exists("dummy")) {
    function dummy() {
        var_dump(__FUNCTION__);
    }
}

dummy();

$dummy = function () { var_dump("lambda" . "dummy"); };
$dummy();

if (!class_exists("A")) {
    class A {
        public static $name = "A";
        public static function say($n = "name") {
            var_dump(static::$name);
        }
    }
}

class B extends A {
    public static $name = "B";
}

if (!class_exists("C")) {
    class C extends B {
        public static $name = "C";
    }
}

A::say();
B::Say();
A::say();
B::say();
C::say();

function get_eol_define() {
    define("MY_EOL", PHP_EOL);
}
get_eol_define();
define("EOL", MY_EOL);

echo "constants define" . ":" . EOL;

echo "define " . "TEST" . EOL;
define("TEST", "TEST");

class E {
    public static $E="EP";
    const E="E";
    const TEST="NULL";
}

class F {
    const F="F";
    public static $E="FEP";
    const E="FE";
    const TEST="FALSE";
    public static $F = "FP";
}

var_dump(TEST);   	//"TEST"
var_dump(E::E); 	//"E"
var_dump(F::E); 	//"FE"
var_dump(F::F); 	//"F"
var_dump(E::TEST);  //"NULL"
var_dump(F::TEST);  //"FALSE"
var_dump(E::$E);	//"EP"
var_dumP(F::$F);    //"FP"
var_dumP(F::$E);    //"FEP"

echo "propertes and methods" . EOL;

class CH {
    const H = "H";
    public function h() {
        var_dump(self::H);
    }
}

class CI {
    const H = "I";
    public function h() {
        var_dump(self::H);
    }
}

function change(&$obj) {
    $obj = new CH;
}

function geti() {
    return new CI;
}

$h = new CH;

echo "-->H" . PHP_EOL;
$h->H();
var_dump($h::H);
var_dump(CH::H);

$h->H();
var_dump($h::H);
var_dump(CH::H);

echo "-->I" . PHP_EOL;
$h = new CI;
$h->H();
var_dump($h::H);
var_dump(CI::H);
$h->H();
var_dump($h::H);
var_dump(CI::H);

echo "-->H" . PHP_EOL;
change($h);

$h->H();
var_dump($h::H);
var_dump(CH::H);

$h->H();
var_dump($h::H);
var_dump(CH::H);

echo "-->I" . PHP_EOL;
$h = geti();
$h->H();
var_dump($h::H);
var_dump(CI::H);
$h->H();
var_dump($h::H);
var_dump(CI::H);
?>
--EXPECT--
array key hash:
Array
(
    [1] => one
    [two] => 2
)
function define:
string(5) "dummy"
string(11) "lambdadummy"
string(1) "A"
string(1) "B"
string(1) "A"
string(1) "B"
string(1) "C"
constants define:
define TEST
string(4) "TEST"
string(1) "E"
string(2) "FE"
string(1) "F"
string(4) "NULL"
string(5) "FALSE"
string(2) "EP"
string(2) "FP"
string(3) "FEP"
propertes and methods
-->H
string(1) "H"
string(1) "H"
string(1) "H"
string(1) "H"
string(1) "H"
string(1) "H"
-->I
string(1) "I"
string(1) "I"
string(1) "I"
string(1) "I"
string(1) "I"
string(1) "I"
-->H
string(1) "H"
string(1) "H"
string(1) "H"
string(1) "H"
string(1) "H"
string(1) "H"
-->I
string(1) "I"
string(1) "I"
string(1) "I"
string(1) "I"
string(1) "I"
string(1) "I"
