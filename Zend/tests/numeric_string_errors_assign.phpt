--TEST--
Invalid numeric string E_WARNINGs and E_NOTICEs, combined assignment operations
--FILE--
<?php

// prevents CT eval
function foxcache($val) {
    return [$val][0];
}

$a = foxcache("2 Lorem");
$a += "3 ipsum";
var_dump($a);
$a = foxcache("dolor");
$a += "sit";
var_dump($a);
echo "---", PHP_EOL;
$a = foxcache("5 amet,");
$a -= "7 consectetur";
var_dump($a);
$a = foxcache("adipiscing");
$a -= "elit,";
var_dump($a);
echo "---", PHP_EOL;
$a = foxcache("11 sed");
$a *= "13 do";
var_dump($a);
$a = foxcache("eiusmod"); 
$a *= "tempor";
var_dump($a);
echo "---", PHP_EOL;
$a = foxcache("17 incididunt");
$a /= "19 ut";
var_dump($a);
$a = foxcache("labore");
$a /= "et";
var_dump($a);
echo "---", PHP_EOL;
$a = foxcache("23 dolore");
$a **= "29 magna";
var_dump($a);
$a = foxcache("aliqua.");
$a **= "Ut";
var_dump($a);
echo "---", PHP_EOL;
$a = foxcache("31 enim");
$a %= "37 ad";
var_dump($a);
try {
    $a = foxcache("minim");
    $a %= "veniam,";
    var_dump($a);
} catch (DivisionByZeroError $e) {
}
echo "---", PHP_EOL;
$a = foxcache("41 minim");
$a <<= "43 veniam,";
var_dump($a);
$a = foxcache("quis");
$a <<= "nostrud";
var_dump($a);
echo "---", PHP_EOL;
$a = foxcache("47 exercitation");
$a >>= "53 ullamco";
var_dump($a);
$a = foxcache("laboris");
$a >>= "nisi";
var_dump($a);
echo "---", PHP_EOL;
$a = foxcache("59 ut");
$a |= 61;
var_dump($a);
$a = foxcache(67);
$a |= "71 aliquip";
var_dump($a);
$a = foxcache("ex");
$a |= 73;
var_dump($a);
$a = foxcache(79);
$a |= "ea";
var_dump($a);
echo "---", PHP_EOL;
$a = foxcache("83 commodo");
$a &= 89;
var_dump($a);
$a = foxcache(97);
$a &= "101 consequat.";
var_dump($a);
$a = foxcache("Duis");
$a &= 103;
var_dump($a);
$a = foxcache(107);
$a &= "aute";
var_dump($a);
echo "---", PHP_EOL;
$a = foxcache("109 irure");
$a ^= 113;
var_dump($a);
$a = foxcache(127);
$a ^= "131 dolor";
var_dump($a);
$a = foxcache("in");
$a ^= 137;
var_dump($a);
$a = foxcache(139);
$a ^= "reprehenderit";
var_dump($a);
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
