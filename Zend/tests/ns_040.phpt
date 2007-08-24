--TEST--
040: Constant declaration and usage in namespace
--FILE--
<?php
namespace X;
import X as Y;
const A = "ok\n";
const B = A;
const C = array(A);
const D = array("aaa"=>A);
const E = array(A=>"aaa\n");
function f1($x=A) {
	echo $x;
}
function f2($x=X::A) {
	echo $x;
}
function f3($x=Y::A) {
	echo $x;
}
function f4($x=::X::A) {
	echo $x;
}
function f5($x=B) {
	echo $x;
}
function f6($x=array(A)) {
	echo $x[0];
}
function f7($x=array("aaa"=>A)) {
	echo $x["aaa"];
}
function f8($x=array(A=>"aaa\n")) {
	echo $x["ok\n"];
}
echo A;
echo X::A;
echo Y::A;
echo ::X::A;
f1();
f2();
f3();
f4();
echo B;
$x = C; echo $x[0];
$x = D; echo $x["aaa"];
$x = E; echo $x["ok\n"];
f5();
f6();
f7();
f8();
--EXPECT--
ok
ok
ok
ok
ok
ok
ok
ok
ok
ok
ok
aaa
ok
ok
ok
aaa
