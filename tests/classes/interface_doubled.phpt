--TEST--
ZE2 An interface extends base interfaces
--FILE--
<?php

interface if_a {
	function f_a();
}

interface if_b {
	function f_b();
}

interface if_c extends if_a, if_b {
	function f_c();
}

interface if_d extends if_a, if_b {
	function f_d();
}

interface if_e {
	function f_d();
}

interface if_f extends /*if_e,*/ if_a, if_b, if_c, if_d /*, if_e*/ {
}

class base {
	function test($class) {
		echo "is_a(" . get_class($this) . ", $class) ". (($this instanceof $class) ? "yes\n" : "no\n");
	}
}

echo "class_a\n";

class class_a extends base implements if_a {
	function f_a() {}
	function f_b() {}
	function f_c() {}
	function f_d() {}
	function f_e() {}
}

$t = new class_a();
echo $t->test('if_a');
echo $t->test('if_b');
echo $t->test('if_c');
echo $t->test('if_d');
echo $t->test('if_e');

echo "class_b\n";

class class_b extends base implements if_a, if_b {
	function f_a() {}
	function f_b() {}
	function f_c() {}
	function f_d() {}
	function f_e() {}
}

$t = new class_b();
echo $t->test('if_a');
echo $t->test('if_b');
echo $t->test('if_c');
echo $t->test('if_d');
echo $t->test('if_e');

echo "class_c\n";

class class_c extends base implements if_c {
	function f_a() {}
	function f_b() {}
	function f_c() {}
	function f_d() {}
	function f_e() {}
}

$t = new class_c();
echo $t->test('if_a');
echo $t->test('if_b');
echo $t->test('if_c');
echo $t->test('if_d');
echo $t->test('if_e');

echo "class_d\n";

class class_d extends base implements if_d{
	function f_a() {}
	function f_b() {}
	function f_c() {}
	function f_d() {}
	function f_e() {}
}

$t = new class_d();
echo $t->test('if_a');
echo $t->test('if_b');
echo $t->test('if_c');
echo $t->test('if_d');
echo $t->test('if_e');

echo "class_e\n";

class class_e extends base implements if_a, if_b, if_c, if_d {
	function f_a() {}
	function f_b() {}
	function f_c() {}
	function f_d() {}
	function f_e() {}
}

$t = new class_e();
echo $t->test('if_a');
echo $t->test('if_b');
echo $t->test('if_c');
echo $t->test('if_d');
echo $t->test('if_e');

echo "class_f\n";

class class_f extends base implements if_e {
	function f_a() {}
	function f_b() {}
	function f_c() {}
	function f_d() {}
	function f_e() {}
}

$t = new class_f();
echo $t->test('if_a');
echo $t->test('if_b');
echo $t->test('if_c');
echo $t->test('if_d');
echo $t->test('if_e');

echo "class_g\n";

class class_g extends base implements if_f {
	function f_a() {}
	function f_b() {}
	function f_c() {}
	function f_d() {}
	function f_e() {}
}

$t = new class_g();
echo $t->test('if_a');
echo $t->test('if_b');
echo $t->test('if_c');
echo $t->test('if_d');
echo $t->test('if_e');

?>
===DONE===
--EXPECT--
class_a
is_a(class_a, if_a) yes
is_a(class_a, if_b) no
is_a(class_a, if_c) no
is_a(class_a, if_d) no
is_a(class_a, if_e) no
class_b
is_a(class_b, if_a) yes
is_a(class_b, if_b) yes
is_a(class_b, if_c) no
is_a(class_b, if_d) no
is_a(class_b, if_e) no
class_c
is_a(class_c, if_a) yes
is_a(class_c, if_b) yes
is_a(class_c, if_c) yes
is_a(class_c, if_d) no
is_a(class_c, if_e) no
class_d
is_a(class_d, if_a) yes
is_a(class_d, if_b) yes
is_a(class_d, if_c) no
is_a(class_d, if_d) yes
is_a(class_d, if_e) no
class_e
is_a(class_e, if_a) yes
is_a(class_e, if_b) yes
is_a(class_e, if_c) yes
is_a(class_e, if_d) yes
is_a(class_e, if_e) no
class_f
is_a(class_f, if_a) no
is_a(class_f, if_b) no
is_a(class_f, if_c) no
is_a(class_f, if_d) no
is_a(class_f, if_e) yes
class_g
is_a(class_g, if_a) yes
is_a(class_g, if_b) yes
is_a(class_g, if_c) yes
is_a(class_g, if_d) yes
is_a(class_g, if_e) no
===DONE===
