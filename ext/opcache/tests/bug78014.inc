<?php
class A {
    function foo() { return 0; }
}
class B extends A {
    const X = UNRESOLVED;
}
class C extends B {
    const X = 42;
    function foo() { return 42; }
}
