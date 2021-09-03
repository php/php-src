--TEST--
Invalid numeric string TypeErrors and E_WARNINGs
--FILE--
<?php

var_dump("2 Lorem" + "3 ipsum");
try {
    var_dump("dolor" + "sit");
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
var_dump("5 amet," - "7 consectetur");
try {
    var_dump("adipiscing" - "elit,");
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
var_dump("11 sed" * "13 do");
try {
    var_dump("eiusmod" * "tempor");
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
var_dump("17 incididunt" / "19 ut");
try {
    var_dump("labore" / "et");
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
var_dump("23 dolore" ** "29 magna");
try {
    var_dump("aliqua." ** "Ut");
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
var_dump("31 enim" % "37 ad");
try {
    var_dump("minim" % "veniam,");
} catch (\TypeError $e) {
    echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
var_dump("41 minim" << "43 veniam,");
try {
    var_dump("quis" << "nostrud");
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
var_dump("47 exercitation" >> "53 ullamco");
try {
    var_dump("laboris" >> "nisi");
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
var_dump("59 ut" | 61);
var_dump(67 | "71 aliquip");
try {
    var_dump("ex" | 73);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(79 | "ea");
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
var_dump("83 commodo" & 89);
var_dump(97 & "101 consequat.");
try {
    var_dump("Duis" & 103);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(107 & "aute");
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
var_dump("109 irure" ^ 113);
var_dump(127 ^ "131 dolor");
try {
    var_dump("in" ^ 137);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(139 ^ "reprehenderit");
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
var_dump(+"149 in");
try {
    var_dump(+"voluptate");
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "---", PHP_EOL;
var_dump(-"151 velit");
try {
    var_dump(-"esse");
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
---

Warning: A non-numeric value encountered in %s on line %d
int(149)
Unsupported operand types: string * int
---

Warning: A non-numeric value encountered in %s on line %d
int(-151)
Unsupported operand types: string * int
