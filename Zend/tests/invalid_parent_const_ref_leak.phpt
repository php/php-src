--TEST--
Leak when using an invalid parent:: reference in a constant definition
--FILE--
<?php

class A {
    const B = parent::C;
}

try {
    A::B;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot access "parent" when current class scope has no parent
