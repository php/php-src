--TEST--
GH-24731 (Assertion failure when a bailout interrupts releasing the previous header_register_callback() callback)
--FILE--
<?php
function f() {}

class C {
    public function __destruct() {
        eval('function f() {}');
    }
}

$c = new C;
$cl = function () use ($c) {};
unset($c);
header_register_callback($cl);
unset($cl);
header_register_callback(function () {});
echo "Unreachable\n";
?>
--EXPECTF--
Fatal error: Cannot redeclare function f() (previously declared in %s:%d) in %s : eval()'d code on line %d
