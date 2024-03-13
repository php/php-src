--TEST--
TokenList: dimensions
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="A B C"/>');
$list = $dom->documentElement->classList;

foreach (range(-1, 3) as $i) {
    echo "--- $i ---\n";
    var_dump($list[$i], isset($list[$i]), empty($list[$i]));
}

echo "--- \"0\" ---\n";
var_dump($list["0"], isset($list["0"]), empty($list["0"]));

echo "--- \"foo\" ---\n";
try {
    var_dump($list["foo"], isset($list["foo"]), empty($list["foo"]));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "--- 1.1 ---\n";
var_dump($list[1.1], isset($list[1.1]), empty($list[1.1]));

echo "--- true ---\n";
var_dump($list[true], isset($list[true]), empty($list[true]));

echo "--- false ---\n";
var_dump($list[false], isset($list[false]), empty($list[false]));

echo "--- ref ---\n";
$tmp = 2;
$ref =& $tmp;
var_dump($list[$ref], isset($list[$ref]), empty($list[$ref]));

?>
--EXPECTF--
--- -1 ---
NULL
bool(false)
bool(true)
--- 0 ---
string(1) "A"
bool(true)
bool(false)
--- 1 ---
string(1) "B"
bool(true)
bool(false)
--- 2 ---
string(1) "C"
bool(true)
bool(false)
--- 3 ---
NULL
bool(false)
bool(true)
--- "0" ---
string(1) "A"
bool(true)
bool(false)
--- "foo" ---
Cannot access offset of type string on DOM\TokenList
--- 1.1 ---

Deprecated: Implicit conversion from float 1.1 to int loses precision in %s on line %d

Deprecated: Implicit conversion from float 1.1 to int loses precision in %s on line %d

Deprecated: Implicit conversion from float 1.1 to int loses precision in %s on line %d
string(1) "B"
bool(true)
bool(false)
--- true ---
string(1) "B"
bool(true)
bool(false)
--- false ---
string(1) "A"
bool(true)
bool(false)
--- ref ---
string(1) "C"
bool(true)
bool(false)
