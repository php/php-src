--TEST--
Class order allowed with autoloading (5)
--FILE--
<?php

// Similar to variance3, but one more class hierarchy in the cycle
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
            public function method(): Q {}
        }
        var_dump(new X);
    } else if ($class == 'Y') {
        class Y extends X {
            public function method(): R {}
        }
        var_dump(new Y);
    } else if ($class == 'Q') {
        class Q {
            public function method(): A {}
        }
        var_dump(new Q);
    } else if ($class == 'R') {
        class R extends Q {
            public function method(): B {}
        }
        var_dump(new R);
    }
});

var_dump(new B);

?>
===DONE===
--EXPECT--
object(A)#2 (0) {
}
object(X)#2 (0) {
}
object(Q)#2 (0) {
}
object(R)#2 (0) {
}
object(Y)#2 (0) {
}
object(B)#2 (0) {
}
object(B)#2 (0) {
}
===DONE===
