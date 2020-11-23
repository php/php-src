--TEST--
Closure 059: Closure type declaration
--FILE--
<?php
class A {
}

class B {
}

$a = new A;
$b = new B;

$f = function (A $a){};

$f($a);
$f->__invoke($a);
call_user_func(array($f,"__invoke"), $a);

try {
    $f($b);
} catch (Error $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
try {
    $f->__invoke($b);
} catch (Error $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
try {
    call_user_func(array($f,"__invoke"), $b);
} catch (Error $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
?>
--EXPECTF--
Exception: {closure}(): Argument #1 ($a) must be of type A, B given, called in %s on line %d
Exception: {closure}(): Argument #1 ($a) must be of type A, B given
Exception: {closure}(): Argument #1 ($a) must be of type A, B given
