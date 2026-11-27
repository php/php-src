--TEST--
PFA in constexpr: shared AST
--FILE--
<?php

trait T {
    static function f($a = self::g(?)) {
        return $a;
    }
}

class C {
    use T;
    static function g($a) {
        printf("%s: %s\n", __CLASS__, $a);
    }
}

class D {
    use T;
    static function g($a) {
        printf("%s: %s\n", __CLASS__, $a);
    }
}

class E {
    use T;
    static function g($a) {
        printf("%s: %s\n", __CLASS__, $a);
    }
}

C::f()(1);
D::f()(2);
E::f()(3);

?>
--EXPECT--
C: 1
D: 2
E: 3
