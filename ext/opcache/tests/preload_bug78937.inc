<?php
function foo() {
    return new class extends Bar {};
}
function bar() {
    class Foo extends Bar {
    }
}
