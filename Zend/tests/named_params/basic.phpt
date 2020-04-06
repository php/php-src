--TEST--
Basic test
--FILE--
<?php

function test($a, $b, $c = "c", $d = "d", $e = "e") {
    echo "a=$a, b=$b, c=$c, d=$d, e=$e\n";
}

function test3(&$a, &$b, &$c = "c", &$d = "d", &$e = "e") {
    echo "a=$a, b=$b, c=$c, d=$d, e=$e\n";
}

function &id($x) {
    return $x;
}

$a = "A"; $b = "B"; $c = "C"; $d = "D"; $e = "E";

echo "SEND_VAL:\n";
test("A", "B", "C", d: "D", e: "E");
test("A", "B", "C", e: "E", d: "D");
test(e: "E", a: "A", d: "D", b: "B", c: "C");
test("A", "B", "C", e: "E");

echo "SEND_VAL_EX:\n";
test2("A", "B", "C", d: "D", e: "E");
test2("A", "B", "C", e: "E", d: "D");
test2(e: "E", a: "A", d: "D", b: "B", c: "C");
test2("A", "B", "C", e: "E");

echo "SEND_VAR:\n";
test($a, $b, $c, d: $d, e: $e);
test($a, $b, $c, e: $e, d: $d);
test(e: $e, a: $a, d: $d, b: $b, c: $c);
test(a: $a, b: $b, c: $c, e: $e);

echo "SEND_VAR_EX:\n";
test2($a, $b, $c, d: $d, e: $e);
test2($a, $b, $c, e: $e, d: $d);
test2(e: $e, a: $a, d: $d, b: $b, c: $c);
test2(a: $a, b: $b, c: $c, e: $e);

echo "SEND_VAR_NO_REF:\n";
test3(id("A"), id("B"), id("C"), d: id("D"), e: id("E"));
test3(id("A"), id("B"), id("C"), e: id("E"), d: id("D"));
test3(e: id("E"), a: id("A"), d: id("D"), b: id("B"), c: id("C"));
test3(id("A"), id("B"), id("C"), e: id("E"));

echo "SEND_VAR_NO_REF_EX:\n";
test4(id("A"), id("B"), id("C"), d: id("D"), e: id("E"));
test4(id("A"), id("B"), id("C"), e: id("E"), d: id("D"));
test4(e: id("E"), a: id("A"), d: id("D"), b: id("B"), c: id("C"));
test4(id("A"), id("B"), id("C"), e: id("E"));

echo "SEND_REF:\n";
test3($a, $b, $c, d: $d, e: $e);
test3($a, $b, $c, e: $e, d: $d);
test3(e: $e, a: $a, d: $d, b: $b, c: $c);
test3(a: $a, b: $b, c: $c, e: $e);

function test2($a, $b, $c = "c", $d = "d", $e = "e") {
    echo "a=$a, b=$b, c=$c, d=$d, e=$e\n";
}

function test4(&$a, &$b, &$c = "c", &$d = "d", &$e = "e") {
    echo "a=$a, b=$b, c=$c, d=$d, e=$e\n";
}

?>
--EXPECT--
SEND_VAL:
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=d, e=E
SEND_VAL_EX:
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=d, e=E
SEND_VAR:
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=d, e=E
SEND_VAR_EX:
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=d, e=E
SEND_VAR_NO_REF:
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=d, e=E
SEND_VAR_NO_REF_EX:
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=d, e=E
SEND_REF:
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=D, e=E
a=A, b=B, c=C, d=d, e=E
