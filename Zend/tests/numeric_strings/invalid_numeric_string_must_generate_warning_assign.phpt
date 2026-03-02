--TEST--
Invalid numeric string TypeErrors and E_WARNINGs, combined assignment operations
--FILE--
<?php

// prevents CT eval
function foxcache($val) {
    return [$val][0];
}

$a = foxcache("2 Lorem");
$a += "3 ipsum";
var_dump($a);
try {
    $a = foxcache("dolor");
    $a += "sit";
    var_dump($a);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
$a = foxcache("5 amet,");
$a -= "7 consectetur";
var_dump($a);
try {
    $a = foxcache("adipiscing");
    $a -= "elit,";
    var_dump($a);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
$a = foxcache("11 sed");
$a *= "13 do";
var_dump($a);
try {
    $a = foxcache("eiusmod");
    $a *= "tempor";
    var_dump($a);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
$a = foxcache("17 incididunt");
$a /= "19 ut";
var_dump($a);
try {
    $a = foxcache("labore");
    $a /= "et";
    var_dump($a);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
$a = foxcache("23 dolore");
$a **= "29 magna";
var_dump($a);
try {
    $a = foxcache("aliqua.");
    $a **= "Ut";
    var_dump($a);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
$a = foxcache("31 enim");
$a %= "37 ad";
var_dump($a);
try {
    $a = foxcache("minim");
    $a %= "veniam,";
    var_dump($a);
} catch (\TypeError $e) {
    echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
$a = foxcache("41 minim");
$a <<= "43 veniam,";
try {
    var_dump($a);
    $a = foxcache("quis");
    $a <<= "nostrud";
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump($a);
echo "---", PHP_EOL;
$a = foxcache("47 exercitation");
$a >>= "53 ullamco";
var_dump($a);
try {
    $a = foxcache("laboris");
    $a >>= "nisi";
    var_dump($a);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
$a = foxcache("59 ut");
$a |= 61;
var_dump($a);
$a = foxcache(67);
$a |= "71 aliquip";
var_dump($a);
try {
    $a = foxcache("ex");
    $a |= 73;
    var_dump($a);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    $a = foxcache(79);
    $a |= "ea";
    var_dump($a);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
$a = foxcache("83 commodo");
$a &= 89;
var_dump($a);
$a = foxcache(97);
$a &= "101 consequat.";
var_dump($a);
try {
    $a = foxcache("Duis");
    $a &= 103;
    var_dump($a);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    $a = foxcache(107);
    $a &= "aute";
    var_dump($a);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
$a = foxcache("109 irure");
$a ^= 113;
var_dump($a);
$a = foxcache(127);
$a ^= "131 dolor";
var_dump($a);
try {
    $a = foxcache("in");
    $a ^= 137;
    var_dump($a);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    $a = foxcache(139);
    $a ^= "reprehenderit";
    var_dump($a);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECTF--
Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
int(5)
Unsupported operand types: string + string
---

Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
int(-2)
Unsupported operand types: string - string
---

Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
int(143)
Unsupported operand types: string * string
---

Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
float(0.8947368421052632)
Unsupported operand types: string / string
---

Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
float(3.0910586430935376E+39)
Unsupported operand types: string ** string
---

Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
int(31)
TypeError: Unsupported operand types: string % string
---

Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
int(%d)
Unsupported operand types: string << string
string(4) "quis"
---

Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
int(0)
Unsupported operand types: string >> string
---

Warning: A non-numeric value encountered in %s on line %d
int(63)

Warning: A non-numeric value encountered in %s on line %d
int(71)
Unsupported operand types: string | int
Unsupported operand types: int | string
---

Warning: A non-numeric value encountered in %s on line %d
int(81)

Warning: A non-numeric value encountered in %s on line %d
int(97)
Unsupported operand types: string & int
Unsupported operand types: int & string
---

Warning: A non-numeric value encountered in %s on line %d
int(28)

Warning: A non-numeric value encountered in %s on line %d
int(252)
Unsupported operand types: string ^ int
Unsupported operand types: int ^ string
