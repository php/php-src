--TEST--
Invalid numeric string E_WARNINGs and E_NOTICEs
--FILE--
<?php

var_dump("2 Lorem" + "3 ipsum");
var_dump("dolor" + "sit");
echo "---", PHP_EOL;
var_dump("5 amet," - "7 consectetur");
var_dump("adipiscing" - "elit,");
echo "---", PHP_EOL;
var_dump("11 sed" * "13 do");
var_dump("eiusmod" * "tempor");
echo "---", PHP_EOL;
var_dump("17 incididunt" / "19 ut");
var_dump("labore" / "et");
echo "---", PHP_EOL;
var_dump("23 dolore" ** "29 magna");
var_dump("aliqua." ** "Ut");
echo "---", PHP_EOL;
var_dump("31 enim" % "37 ad");
try {
    var_dump("minim" % "veniam,");
} catch (DivisionByZeroError $e) {
}
echo "---", PHP_EOL;
var_dump("41 minim" << "43 veniam,");
var_dump("quis" << "nostrud");
echo "---", PHP_EOL;
var_dump("47 exercitation" >> "53 ullamco");
var_dump("laboris" >> "nisi");
echo "---", PHP_EOL;
var_dump("59 ut" | 61);
var_dump(67 | "71 aliquip");
var_dump("ex" | 73);
var_dump(79 | "ea");
echo "---", PHP_EOL;
var_dump("83 commodo" & 89);
var_dump(97 & "101 consequat.");
var_dump("Duis" & 103);
var_dump(107 & "aute");
echo "---", PHP_EOL;
var_dump("109 irure" ^ 113);
var_dump(127 ^ "131 dolor");
var_dump("in" ^ 137);
var_dump(139 ^ "reprehenderit");
echo "---", PHP_EOL;
var_dump(+"149 in");
var_dump(+"voluptate");
echo "---", PHP_EOL;
var_dump(-"151 velit");
var_dump(-"esse");
?>
--EXPECTF--

Notice: A non well formed numeric value encountered in %s on line %d

Notice: A non well formed numeric value encountered in %s on line %d
int(5)

Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
int(0)
---

Notice: A non well formed numeric value encountered in %s on line %d

Notice: A non well formed numeric value encountered in %s on line %d
int(-2)

Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
int(0)
---

Notice: A non well formed numeric value encountered in %s on line %d

Notice: A non well formed numeric value encountered in %s on line %d
int(143)

Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
int(0)
---

Notice: A non well formed numeric value encountered in %s on line %d

Notice: A non well formed numeric value encountered in %s on line %d
float(0.89473684210526)

Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d

Warning: Division by zero in %s on line %d
float(NAN)
---

Notice: A non well formed numeric value encountered in %s on line %d

Notice: A non well formed numeric value encountered in %s on line %d
float(3.0910586430935E+39)

Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
int(1)
---

Notice: A non well formed numeric value encountered in %s on line %d

Notice: A non well formed numeric value encountered in %s on line %d
int(31)

Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
---

Notice: A non well formed numeric value encountered in %s on line %d

Notice: A non well formed numeric value encountered in %s on line %d
int(%d)

Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
int(0)
---

Notice: A non well formed numeric value encountered in %s on line %d

Notice: A non well formed numeric value encountered in %s on line %d
int(0)

Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
int(0)
---

Notice: A non well formed numeric value encountered in %s on line %d
int(63)

Notice: A non well formed numeric value encountered in %s on line %d
int(71)

Warning: A non-numeric value encountered in %s on line %d
int(73)

Warning: A non-numeric value encountered in %s on line %d
int(79)
---

Notice: A non well formed numeric value encountered in %s on line %d
int(81)

Notice: A non well formed numeric value encountered in %s on line %d
int(97)

Warning: A non-numeric value encountered in %s on line %d
int(0)

Warning: A non-numeric value encountered in %s on line %d
int(0)
---

Notice: A non well formed numeric value encountered in %s on line %d
int(28)

Notice: A non well formed numeric value encountered in %s on line %d
int(252)

Warning: A non-numeric value encountered in %s on line %d
int(137)

Warning: A non-numeric value encountered in %s on line %d
int(139)
---

Notice: A non well formed numeric value encountered in %s on line %d
int(149)

Warning: A non-numeric value encountered in %s on line %d
int(0)
---

Notice: A non well formed numeric value encountered in %s on line %d
int(-151)

Warning: A non-numeric value encountered in %s on line %d
int(0)
