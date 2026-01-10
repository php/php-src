--TEST--
GH-16198: Incorrect trait constant conflict when declared via trait
--FILE--
<?php

trait T1 {
    final public const string C1 = 'T1';
}

interface I1 {
    public const ?string C1 = null;
    public const ?string C2 = null;
}

final class O1 implements I1 {
    final public const string C2 = 'O1';
}

final class O2 implements I1 {
    use T1;
}

abstract class A1 implements I1 {}

final class O3 extends A1 {
    final public const string C2 = 'O3';
}

final class O4 extends A1 {
    use T1;
}

?>
===DONE===
--EXPECT--
===DONE===
