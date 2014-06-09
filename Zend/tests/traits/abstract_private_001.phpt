--TEST--
abstract private functions in traits (normal case)
--FILE--
<?php

trait T {
    abstract private function f();
}

class C {
    use T;

    public function test() {
        $this->f();
    }

    private function f() {
        echo 'f() called';
    }
}

(new C)->test();

?>
--EXPECT--
f() called
