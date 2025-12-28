--TEST--
Closure 058: Closure scope and object
--FILE--
<?php
class A {
    static function foo() {
        return function () {var_dump(self::class,get_called_class());};
    }
    function bar() {
        return function () {var_dump(self::class,get_called_class(),$this);};
    }
}
$z = "call_user_func";

$a = A::foo();
$a();
$a->__invoke();
$c = array($a,"__invoke");
$c();
call_user_func(array($a,"__invoke"));
$z(array($a,"__invoke"));

echo "\n";

$x = new A();
$b = $x->bar();
$b();
$b->__invoke();
$c = array($b,"__invoke");
$c();
call_user_func(array($b,"__invoke"));
$z(array($b,"__invoke"));
?>
--EXPECT--
string(1) "A"
string(1) "A"
string(1) "A"
string(1) "A"
string(1) "A"
string(1) "A"
string(1) "A"
string(1) "A"
string(1) "A"
string(1) "A"

string(1) "A"
string(1) "A"
object(A)#2 (0) {
}
string(1) "A"
string(1) "A"
object(A)#2 (0) {
}
string(1) "A"
string(1) "A"
object(A)#2 (0) {
}
string(1) "A"
string(1) "A"
object(A)#2 (0) {
}
string(1) "A"
string(1) "A"
object(A)#2 (0) {
}
