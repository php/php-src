--TEST--
Defining a constant in both trait and its composing class with the same name, visibility, finality and value is allowed
--FILE--
<?php

trait TestTrait1 {
    public const A = 42;
}

trait TestTrait2 {
    public const A = 42;
}

trait TestTrait3 {
    use TestTrait2;
    public const A = 42;
}

class ComposingClass1 {
    use TestTrait1;
    use TestTrait2;
}

class ComposingClass2 {
    use TestTrait1;
    use TestTrait3;
}

class ComposingClass3 {
    use TestTrait1;
    use TestTrait3;
    public const A = 42;
}

echo ComposingClass1::A, PHP_EOL;
echo ComposingClass2::A, PHP_EOL;
echo ComposingClass3::A, PHP_EOL;
?>
--EXPECTF--
42
42
42
