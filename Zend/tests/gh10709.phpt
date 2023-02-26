--TEST--
GH-10634: Fix recursive class constant evaluation
--FILE--
<?php

class B { const HW = A::HW . " extended by B"; }

spl_autoload_register(function ($class) {
    class A { const HW = "this is A"; }
    var_dump(B::HW);
});

try {
    var_dump(new B());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot declare self-referencing constant B::HW
