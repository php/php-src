--TEST--
Grouped 'from' syntax test
--FILE--
<?php

namespace My\Full {
    class ClassA { public static function id(){ return "A"; } }
    class ClassB { public static function id(){ return "B"; } }
    class ClassC { public static function id(){ return "C"; } }
}

namespace {
    // Grouped form with 'from' after braces
    use {ClassA, ClassB, ClassC as C} from My\Full;
    echo ClassA::id() . "\n";
    echo ClassB::id() . "\n";
    echo C::id() . "\n";

    // Equivalent grouped form (existing syntax) for comparison
    use My\Full\{ClassA as A2, ClassB as B2, ClassC as C2};
    echo A2::id() . "\n";
    echo B2::id() . "\n";
    echo C2::id() . "\n";

    // Inline aliases and fully-qualified namespace
    use {ClassA as AA, ClassB as BB} from \My\Full;
    echo AA::id() . "\n";
    echo BB::id() . "\n";
}

?>
--EXPECT--
A
B
C
A
B
C
A
B
