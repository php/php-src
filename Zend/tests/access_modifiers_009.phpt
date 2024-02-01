--TEST--
Inconsistencies when accessing protected members - is_callable
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
        var_dump(is_callable('A::ma'));
        var_dump(is_callable('A::mp'));
        var_dump(is_callable('B1::ma')); // protected method defined also in A
        var_dump(is_callable('B1::mp')); // protected method defined also in A but as private
        var_dump(is_callable('B1::mb'));
    }
}

var_dump(is_callable('B2::ma'));
var_dump(is_callable('B2::mp'));
var_dump(is_callable('B2::mb'));
var_dump(is_callable('B2::test'));
echo '----' . "\n";
B2::test();

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(true)
----
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
