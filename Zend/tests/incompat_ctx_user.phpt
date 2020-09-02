--TEST--
Incompatible context call (non-internal function)
--FILE--
<?php

class A {
    function foo() { var_dump(get_class($this)); }
}
class B {
    function bar() { A::foo(); }
}
$b = new B;
try {
    $b->bar();
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Exception: Non-static method A::foo() cannot be called statically
