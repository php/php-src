--TEST--
Class order allowed with autoloading (1)
--FILE--
<?php

spl_autoload_register(function($class) {
    if ($class === 'A') {
        class A {
            public function method() : B {}
        }
        var_dump(new A);
    } else if ($class == 'B') {
        class B extends A {
            public function method() : C {}
        }
        var_dump(new B);
    } else {
        class C extends B {
        }
        var_dump(new C);
    }
});

var_dump(new C);

?>
--EXPECT--
object(A)#2 (0) {
}
object(B)#2 (0) {
}
object(C)#2 (0) {
}
object(C)#2 (0) {
}
