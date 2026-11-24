--TEST--
PFA in constexpr: shared AST (closures)
--FILE--
<?php

$f = eval(<<<'PHP'
return function ($a = self::f(?)) {
    return $a;
};
PHP);

class C {
    static function f($a) {
        printf("%s: %s\n", __CLASS__, $a);
    }
}

class D {
    static function f($a) {
        printf("%s: %s\n", __CLASS__, $a);
    }
}

class E {
    static function f($a) {
        printf("%s: %s\n", __CLASS__, $a);
    }
}

$f->bindTo(null, C::class)()(1);
$f->bindTo(null, D::class)()(2);
$f->bindTo(null, E::class)()(3);

$f = null;

?>
--EXPECT--
C: 1
D: 2
E: 3
