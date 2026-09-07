--TEST--
Bug #76869 (Incorrect bypassing protected method accessibility check)
--FILE--
<?php
class A {
    private function f() {
        return "A";
    }
}
class B extends A {
    protected function f() {
        return "B";
    }
}
$b = new B();
try {
    var_dump($b->f());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Call to protected method B::f() from global scope
