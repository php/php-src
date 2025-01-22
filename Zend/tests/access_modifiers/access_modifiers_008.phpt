--TEST--
Inconsistencies when accessing protected members
--FILE--
<?php

class A {
    static protected function ma() {
        return 'A::ma()';
    }

    static private function mp() {
        return 'A::mp()';
    }
}

class B1 extends A {
    static protected function ma() {
        return 'B1::ma()';
    }

    static protected function mp() {
        return 'B1::mp()';
    }

    static protected function mb() {
        return 'B1::mb()';
    }
}

class B2 extends A {
    static public function test() {
        echo A::ma() . "\n";
        try {
            echo A::mp() . "\n";
        } catch (\Throwable $e) {
            echo $e->getMessage() . "\n";
        }
        echo B1::ma() . "\n"; // protected method defined also in A
        try {
            echo B1::mp() . "\n"; // protected method defined also in A but as private
        } catch (\Throwable $e) {
            echo $e->getMessage() . "\n";
        }
        try {
            echo B1::mb() . "\n";
        } catch (\Throwable $e) {
            echo $e->getMessage() . "\n";
        }
    }
}

B2::test();

?>
--EXPECT--
A::ma()
Call to private method A::mp() from scope B2
B1::ma()
Call to protected method B1::mp() from scope B2
Call to protected method B1::mb() from scope B2
