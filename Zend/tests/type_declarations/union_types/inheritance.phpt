--TEST--
Various inheritance scenarios for properties/methods with union types
--FILE--
<?php

class X {
    public A|B|int $prop;
    public function method(A|B|int $arg): A|B|int { }

    private A|B|int $prop2;
    private function method2(A|B|int $arg): A|B|int { }
}

class Y extends X {
}

trait T {
    public A|B|int $prop;
    public function method(A|B|int $arg): A|B|int { }

    private A|B|int $prop2;
    private function method2(A|B|int $arg): A|B|int { }
}

class Z {
    use T;
}

class U extends X {
    use T;
}

class V extends X {
    use T;

    public A|B|int $prop;
    public function method(A|B|int $arg): A|B|int { }

    private A|B|int $prop2;
    private function method2(A|B|int $arg): A|B|int { }
}

?>
===DONE===
--EXPECT--
===DONE===
