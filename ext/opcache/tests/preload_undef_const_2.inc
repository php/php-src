<?php
trait T {
    public function test() {
        return 123;
    }
}

class Foo {
    const C = UNDEF;
    use T;
}
