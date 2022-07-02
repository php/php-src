--TEST--
Class order allowed with autoloading (3)
--FILE--
<?php

spl_autoload_register(function($class) {
    if ($class == 'A') {
        class A {
            public function method(): X {}
        }
        var_dump(new A);
    } else if ($class == 'B') {
        class B extends A {
            public function method(): Y {}
        }
        var_dump(new B);
    } else if ($class == 'X') {
        class X {
            public function method(): A {}
        }
        var_dump(new X);
    } else if ($class == 'Y') {
        class Y extends X {
            public function method(): B {}
        }
        var_dump(new Y);
    }
});

var_dump(new B);

?>
--EXPECT--
object(A)#2 (0) {
}
object(X)#2 (0) {
}
object(Y)#2 (0) {
}
object(B)#2 (0) {
}
object(B)#2 (0) {
}
